/*
 * sound.h - The sound effects
 *           (Java: com.mojang.ld22.sound.Sound).
 *
 * Original Java:
 *   public static final Sound playerHurt  = new Sound("/playerhurt.wav");
 *   public static final Sound playerDeath = new Sound("/death.wav");
 *   public static final Sound monsterHurt = new Sound("/monsterhurt.wav");
 *   public static final Sound test        = new Sound("/test.wav");
 *   public static final Sound pickup      = new Sound("/pickup.wav");
 *   public static final Sound bossdeath   = new Sound("/bossdeath.wav");
 *   public static final Sound craft       = new Sound("/craft.wav");
 *
 * The WAV samples are embedded in the binary at compile time by
 * scripts/sound2c.py (see source/extern/sound_data.c).
 *
 * Playback uses a tiny software mixer on top of the SDL audio callback: it
 * works with both SDL 1.2 and SDL2 and needs no SDL_mixer, which keeps it
 * friendly to embedded targets. Define NO_AUDIO=1 to compile audio out.
 */
#ifndef SOUND_SOUND_H_
#define SOUND_SOUND_H_ 1

#include <stdint.h>

#include "../utils/javalang.h"

/*
 * Which sound is which. C-only: Java holds one Sound instance per constant,
 * so it needs no ids.
 */
typedef enum SoundId {
    /* Sound.playerHurt  -> playerhurt.wav  */
    SND_PLAYERHURT = 0,
    /* Sound.playerDeath -> death.wav       */
    SND_PLAYERDEATH,
    /* Sound.monsterHurt -> monsterhurt.wav */
    SND_MONSTERHURT,
    /* Sound.test        -> test.wav        */
    SND_TEST,
    /* Sound.pickup      -> pickup.wav      */
    SND_PICKUP,
    /* Sound.bossdeath   -> bossdeath.wav   */
    SND_BOSSDEATH,
    /* Sound.craft       -> craft.wav       */
    SND_CRAFT,
    SOUND_COUNT
} SoundId;

/* One packed sound effect; the table is generated into sound_data.c. */
typedef struct SoundData SoundData;

struct SoundData {
    /* Mono 16-bit signed PCM. */
    const int16_t* samples;
    /* Number of samples. */
    unsigned int length;
    /* Samples per second (44100). */
    unsigned int rate;
};

extern const SoundData sound_data_table[SOUND_COUNT];

/*
 * Initializes the audio subsystem and opens the mixing device.
 *
 * Never fatal: it returns true when audio is active and false when it is not
 * (headless or embedded without sound, or a NO_AUDIO build), and the game
 * keeps running silently either way.
 */
PUBLIC boolean sound_init(void);

/* Closes the audio device and quits the audio subsystem. */
PUBLIC void sound_quit(void);

/* Plays a sound, as Java's Sound.play() does. Safe to call when audio is
 * off. */
PUBLIC void sound_play(SoundId id);

#endif /* SOUND_SOUND_H_ */
