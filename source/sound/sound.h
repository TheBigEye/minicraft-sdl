/*
 * sound.h — C port of com.mojang.ld22.sound.Sound (Minicraft, LD22)
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
 * scripts/sound2c.py (see source/generated/sound_data.c).
 *
 * Playback uses a tiny software mixer on top of the SDL audio callback
 * (works with both SDL 1.2 and SDL2, no SDL_mixer dependency, embedded
 * friendly). Define NO_AUDIO=1 at build time to compile audio out entirely.
 */

#ifndef SOUND_SOUND_H_
#define SOUND_SOUND_H_

#include <stdint.h>

typedef enum {
	SND_PLAYERHURT = 0,   /* Sound.playerHurt  → playerhurt.wav  */
	SND_PLAYERDEATH,      /* Sound.playerDeath → death.wav       */
	SND_MONSTERHURT,      /* Sound.monsterHurt → monsterhurt.wav */
	SND_TEST,             /* Sound.test        → test.wav        */
	SND_PICKUP,           /* Sound.pickup      → pickup.wav      */
	SND_BOSSDEATH,        /* Sound.bossdeath   → bossdeath.wav   */
	SND_CRAFT,            /* Sound.craft       → craft.wav       */
	SOUND_COUNT
} SoundId;

/* One packed sound effect (defined by generated sound_data.c) */
typedef struct {
	const int16_t* samples;   /* mono 16-bit signed PCM           */
	unsigned int length;      /* number of samples                */
	unsigned int rate;        /* samples per second (44100)       */
} SoundData;

extern const SoundData sound_data_table[SOUND_COUNT];

/*
 * Initialize the audio subsystem and open the mixing device.
 * Never fatal: returns 1 if audio is active, 0 if unavailable
 * (headless / embedded without sound / NO_AUDIO build) and the
 * game keeps running silently.
 */
int sound_init(void);

/* Close the audio device and quit the audio subsystem. */
void sound_quit(void);

/* Play a sound (like Java's Sound.play()). Safe to call when audio is off. */
void sound_play(SoundId id);

#endif /* SOUND_SOUND_H_ */
