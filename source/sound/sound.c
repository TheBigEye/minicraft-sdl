/*
 * sound.c - A tiny software mixer over the SDL audio callback
 *           (Java: com.mojang.ld22.sound.Sound).
 *
 * It works with SDL 1.2, SDL2 and SDL3. The first two share the
 * SDL_OpenAudio()/SDL_LockAudio() pair; SDL3 dropped both and hands out an
 * SDL_AudioStream instead, which the mixer feeds from its callback. Either
 * way it asks for mono 16-bit at 44100 Hz, the format of the embedded
 * assets, and SDL converts internally to whatever the hardware needs.
 *
 * The original Java Sound.play() spawned a thread per play and let the
 * applet audio clips overlap freely; here up to SOUND_MAX_VOICES instances
 * are mixed at once, stealing the oldest voice when they are all busy.
 */
#include "sound.h"

#include <stdio.h>
#include <string.h>

#include "../log.h"

#ifdef NO_AUDIO

/* -------------- audio compiled out (embedded targets and so on) -------------- */

PUBLIC boolean sound_init(void) {
    return false;
}

PUBLIC void sound_quit(void) {
}

PUBLIC void sound_play(SoundId id) {
    (void) id;
}

#else

#include "../sdlcompat.h"

#define SOUND_MAX_VOICES 8

/* One playing instance of a sound. */
typedef struct Voice Voice;

struct Voice {
    const int16_t* data;
    /* Total samples. */
    unsigned int length;
    /* Current sample. */
    unsigned int pos;
    boolean active;
};

PRIVATE Voice voices[SOUND_MAX_VOICES];
/* Round-robin slot used when stealing a voice. */
PRIVATE int next_voice = 0;
PRIVATE boolean sound_active = false;

/* SDL3 keeps the device behind an audio stream; the other two do not. */
#ifdef USE_SDL3
    /*
     * Frames the callback mixes per call. SDL3 asks for "roughly" the bytes
     * it needs and is happy with less, so a fixed block is enough; audio
     * callbacks are serialised by SDL, which is what makes one shared
     * buffer safe.
     */
    #define SOUND_CHUNK_FRAMES 2048

    PRIVATE SDL_AudioStream* sound_stream = null;

    /* Scratch area the callback mixes into. */
    PRIVATE int16_t sound_chunk[SOUND_CHUNK_FRAMES];
#endif


/* Mixes every active voice into `out`, `frames` mono 16-bit samples. */
PRIVATE void sound_mix(int16_t* out, int frames) {
    memset(out, 0, (size_t) frames * sizeof(int16_t));

    for (int v = 0; v < SOUND_MAX_VOICES; ++v) {
        Voice* voice = &voices[v];

        if (!voice->active) continue;

        for (int i = 0; i < frames; ++i) {
            if (voice->pos >= voice->length) {
                voice->active = false;
                break;
            }

            int32_t mixed = (int32_t) out[i] + (int32_t) voice->data[voice->pos++];

            if (mixed > 32767) mixed = 32767;
            else if (mixed < -32768) mixed = -32768;

            out[i] = (int16_t) mixed;
        }
    }
}


#ifdef USE_SDL3

/*
 * Runs on the SDL audio thread. SDL3 has no fixed buffer to write into:
 * the stream asks for "additional_amount" bytes in *our* format (mono
 * 16-bit 44100 Hz) and converts them to whatever the device wants. Supplying
 * fewer bytes than requested is explicitly allowed, so the fixed chunk above
 * is fine no matter how much the device asks for.
 */
PRIVATE void SDLCALL sound_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
    (void) userdata;
    (void) total_amount;

    if (additional_amount <= 0) return;

    int frames = additional_amount / (int) sizeof(int16_t);

    if (frames > SOUND_CHUNK_FRAMES) frames = SOUND_CHUNK_FRAMES;
    if (frames <= 0) return;

    sound_mix(sound_chunk, frames);

    SDL_PutAudioStreamData(stream, sound_chunk, frames * (int) sizeof(int16_t));
}

#else

/* Runs on the SDL audio thread: mixes every active voice into the stream. */
PRIVATE void sound_callback(void* userdata, Uint8* stream, int len) {
    (void) userdata;

    sound_mix((int16_t*) stream, len / (int) sizeof(int16_t));
}

#endif /* USE_SDL3 */


PUBLIC boolean sound_init(void) {
    memset(voices, 0, sizeof(voices));

    if (!SDL_INIT_SUCCEEDED(SDL_InitSubSystem(SDL_INIT_AUDIO))) {
        LOG_WARN("audio init failed, continuing without sound: %s", SDL_GetError());
        return false;
    }

#ifdef USE_SDL3
    /*
     * SDL3: SDL_AudioSpec carries only the format SDL should convert from,
     * and the device comes back as a stream. It starts paused, hence the
     * resume. Destroying the stream later closes the device with it.
     */
    SDL_AudioSpec spec;

    memset(&spec, 0, sizeof(spec));

    spec.freq = 44100;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;

    sound_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, sound_callback, null);

    if (!sound_stream) {
        LOG_WARN("could not open audio stream, continuing without sound: %s", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    sound_active = true;

    SDL_ResumeAudioStreamDevice(sound_stream);

    LOG_INFO("audio ready: %d Hz, %d ch, SDL3 audio stream", spec.freq, spec.channels);

#else
    SDL_AudioSpec desired;
    SDL_AudioSpec obtained;

    memset(&desired, 0, sizeof(desired));

    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 1;
    desired.samples = 1024;   /* about 23 ms of buffer */
    desired.callback = sound_callback;
    desired.userdata = null;

    if (SDL_OpenAudio(&desired, &obtained) != 0) {
        LOG_WARN("could not open audio device, continuing without sound: %s", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    sound_active = true;
    SDL_PauseAudio(0);

    LOG_INFO("audio ready: %d Hz, %d ch, %d samples buffer", obtained.freq, obtained.channels, obtained.samples);
#endif

    return true;
}


PUBLIC void sound_quit(void) {
    if (!sound_active) return;

    sound_active = false;

#ifdef USE_SDL3
    /* Destroying the stream closes the device bound to it. */
    SDL_DestroyAudioStream(sound_stream);
    sound_stream = null;
#else
    SDL_PauseAudio(1);
    SDL_CloseAudio();
#endif

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}


PUBLIC void sound_play(SoundId id) {
    if (!sound_active) return;
    if ((int) id < 0 || id >= SOUND_COUNT) return;

    const SoundData* snd = &sound_data_table[id];

    if (!snd->samples || snd->length == 0) return;

#ifdef USE_SDL3
    SDL_LockAudioStream(sound_stream);
#else
    SDL_LockAudio();
#endif

    /*
     * Find a free voice; if there is none, steal one round-robin. Like the
     * Java version, sounds never block gameplay, they just overlap.
     */
    int slot = -1;

    for (int i = 0; i < SOUND_MAX_VOICES; ++i) {
        if (!voices[i].active) {
            slot = i;
            break;
        }
    }

    if (slot < 0) {
        slot = next_voice;
        next_voice = (next_voice + 1) % SOUND_MAX_VOICES;
    }

    voices[slot].data = snd->samples;
    voices[slot].length = snd->length;
    voices[slot].pos = 0;
    voices[slot].active = true;

#ifdef USE_SDL3
    SDL_UnlockAudioStream(sound_stream);
#else
    SDL_UnlockAudio();
#endif
}

#endif /* NO_AUDIO */
