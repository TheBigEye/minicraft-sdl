/*
 * sound.c — tiny software mixer over the SDL audio callback.
 *
 * Works with SDL 1.2 and SDL2 (SDL_OpenAudio + SDL_LockAudio exist in both).
 * We request mono 16-bit @ 44100 Hz (the format of the embedded assets);
 * SDL converts internally to whatever the hardware needs.
 *
 * The original Java Sound.play() spawned a thread per play and let the
 * applet audio clip overlap freely; here we mix up to SOUND_MAX_VOICES
 * concurrent instances, stealing the oldest voice when full.
 */

#include "sound.h"

#include <stdio.h>
#include <string.h>

#ifdef NO_AUDIO

/* -------------- audio compiled out (embedded targets etc.) -------------- */

int sound_init(void) { return 0; }
void sound_quit(void) { }
void sound_play(SoundId id) { (void)id; }

#else

#ifdef USE_SDL1
	#include <SDL/SDL.h>
#else
	#include <SDL2/SDL.h>
#endif

#define SOUND_MAX_VOICES 8

typedef struct {
	const int16_t* data;
	unsigned int length;   /* total samples          */
	unsigned int pos;      /* current sample         */
	int active;
} Voice;

static Voice voices[SOUND_MAX_VOICES];
static int next_voice = 0;        /* round-robin slot for voice stealing */
static int sound_active = 0;

/* Runs on the SDL audio thread: mix all active voices into the stream. */
static void sound_callback(void* userdata, Uint8* stream, int len) {
	(void)userdata;

	int16_t* out = (int16_t*) stream;
	int frames = len / (int) sizeof(int16_t);

	memset(out, 0, (size_t) len);

	for (int v = 0; v < SOUND_MAX_VOICES; ++v) {
		Voice* voice = &voices[v];
		if (!voice->active) continue;

		for (int i = 0; i < frames; ++i) {
			if (voice->pos >= voice->length) {
				voice->active = 0;
				break;
			}

			int32_t mixed = (int32_t) out[i] + (int32_t) voice->data[voice->pos++];
			if (mixed > 32767) mixed = 32767;
			else if (mixed < -32768) mixed = -32768;
			out[i] = (int16_t) mixed;
		}
	}
}

int sound_init(void) {
	memset(voices, 0, sizeof(voices));

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		printf("Audio init failed (continuing without sound): %s\n", SDL_GetError());
		return 0;
	}

	SDL_AudioSpec desired;
	SDL_AudioSpec obtained;
	memset(&desired, 0, sizeof(desired));

	desired.freq = 44100;
	desired.format = AUDIO_S16SYS;
	desired.channels = 1;
	desired.samples = 1024;         /* ~23 ms buffer */
	desired.callback = sound_callback;
	desired.userdata = NULL;

	if (SDL_OpenAudio(&desired, &obtained) != 0) {
		printf("Could not open audio device (continuing without sound): %s\n", SDL_GetError());
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return 0;
	}

	sound_active = 1;
	SDL_PauseAudio(0);

	printf("Audio ready: %d Hz, %d ch, %d samples buffer\n",
	       obtained.freq, obtained.channels, obtained.samples);
	return 1;
}

void sound_quit(void) {
	if (!sound_active) return;
	sound_active = 0;

	SDL_PauseAudio(1);
	SDL_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void sound_play(SoundId id) {
	if (!sound_active) return;
	if ((int) id < 0 || id >= SOUND_COUNT) return;

	const SoundData* snd = &sound_data_table[id];
	if (!snd->samples || snd->length == 0) return;

	SDL_LockAudio();

	/* Find a free voice; if none, steal one round-robin (like the Java
	 * version, sounds never block gameplay — they just overlap). */
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
	voices[slot].active = 1;

	SDL_UnlockAudio();
}

#endif /* NO_AUDIO */
