#!/usr/bin/env python3
"""
sound2c.py - Pack WAV sound effects into a C source file (embedded in binary).

Reads standard PCM WAV files with the stdlib `wave` module (no dependencies),
normalizes them all to mono / 16-bit / 44100 Hz (downmix + linear resample
done HERE, at build time, so the in-game mixer stays tiny and fast - ideal
for embedded targets), and emits a table indexed by the SoundId enum from
source/sound/sound.h.

Usage:
    python3 scripts/sound2c.py OUTPUT.c INPUT1.wav [INPUT2.wav ...]

Example:
    python3 scripts/sound2c.py source/extern/sound_data.c assets/*.wav

File name (without extension) → SoundId mapping:
    playerhurt.wav  → SND_PLAYERHURT     (Sound.playerHurt)
    death.wav       → SND_PLAYERDEATH    (Sound.playerDeath)
    monsterhurt.wav → SND_MONSTERHURT    (Sound.monsterHurt)
    test.wav        → SND_TEST           (Sound.test)
    pickup.wav      → SND_PICKUP         (Sound.pickup)
    bossdeath.wav   → SND_BOSSDEATH      (Sound.bossdeath)
    craft.wav       → SND_CRAFT          (Sound.craft)
Unknown file names are ignored (with a warning).
"""

import os
import sys
import wave

TARGET_RATE = 44100

# asset file basename -> (SoundId enum name, original Java field name)
NAME_MAP = {
    "playerhurt":  ("SND_PLAYERHURT",  "Sound.playerHurt"),
    "death":       ("SND_PLAYERDEATH", "Sound.playerDeath"),
    "monsterhurt": ("SND_MONSTERHURT", "Sound.monsterHurt"),
    "test":        ("SND_TEST",        "Sound.test"),
    "pickup":      ("SND_PICKUP",      "Sound.pickup"),
    "bossdeath":   ("SND_BOSSDEATH",   "Sound.bossdeath"),
    "craft":       ("SND_CRAFT",       "Sound.craft"),
}

VALUES_PER_LINE = 16


def load_wav_normalized(path):
    """Return (samples, rate) as mono signed 16-bit at TARGET_RATE."""
    with wave.open(path, "rb") as w:
        nchannels = w.getnchannels()
        sampwidth = w.getsampwidth()
        rate = w.getframerate()
        nframes = w.getnframes()
        comp = w.getcomptype()
        raw = w.readframes(nframes)

    if comp != "NONE":
        raise ValueError("%s: only uncompressed PCM WAV supported (got %r)" % (path, comp))

    # --- decode samples to list[int] in [-32768, 32767] ---
    if sampwidth == 1:  # unsigned 8-bit
        frames = [((b - 128) << 8) for b in raw]
    elif sampwidth == 2:  # signed 16-bit little endian (standard)
        frames = list(_iter_int16_le(raw))
    elif sampwidth == 3:  # signed 24-bit little endian
        frames = list(_iter_int24_le(raw))
    elif sampwidth == 4:  # signed 32-bit little endian
        frames = list(_iter_int32_le(raw))
    else:
        raise ValueError("%s: unsupported sample width %d bytes" % (path, sampwidth))

    # --- downmix to mono ---
    if nchannels > 1:
        mono = []
        for i in range(0, len(frames), nchannels):
            chunk = frames[i : i + nchannels]
            mono.append(sum(chunk) // len(chunk))
        frames = mono

    # --- resample to TARGET_RATE (linear interpolation) ---
    if rate != TARGET_RATE:
        src_len = len(frames)
        dst_len = int(round(src_len * TARGET_RATE / rate))
        resampled = []
        step = rate / TARGET_RATE
        pos = 0.0
        for _ in range(dst_len):
            i0 = int(pos)
            frac = pos - i0
            if i0 + 1 < src_len:
                s = frames[i0] * (1.0 - frac) + frames[i0 + 1] * frac
            else:
                s = frames[min(i0, src_len - 1)]
            resampled.append(int(round(s)))
            pos += step
        frames = resampled

    return frames, TARGET_RATE


def _iter_int16_le(raw):
    for i in range(0, len(raw) - 1, 2):
        v = raw[i] | (raw[i + 1] << 8)
        yield v - 65536 if v >= 32768 else v


def _iter_int24_le(raw):
    for i in range(0, len(raw) - 2, 3):
        v = raw[i] | (raw[i + 1] << 8) | (raw[i + 2] << 16)
        yield (v - (1 << 24) if v >= (1 << 23) else v) >> 8


def _iter_int32_le(raw):
    for i in range(0, len(raw) - 3, 4):
        v = raw[i] | (raw[i + 1] << 8) | (raw[i + 2] << 16) | (raw[i + 3] << 24)
        yield (v - (1 << 32) if v >= (1 << 31) else v) >> 16


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 1

    out_path = argv[1]
    entries = {}  # enum name -> (array_name, samples, rate, source_file)

    for path in argv[2:]:
        base = os.path.splitext(os.path.basename(path))[0].lower()
        if base not in NAME_MAP:
            print("sound2c: WARNING: unknown asset %r - skipped" % path, file=sys.stderr)
            continue
        enum_name, java_name = NAME_MAP[base]
        samples, rate = load_wav_normalized(path)
        entries[enum_name] = (
            "snd_%s_samples" % base, samples, rate, path, java_name,
        )
        print("sound2c: %s -> %s (%d samples @ %d Hz)" % (path, enum_name, len(samples), rate))

    missing = [e for e, (_j) in NAME_MAP.items() if e[1] not in entries]
    for enum_name, java_name in NAME_MAP.values():
        if enum_name not in entries:
            print("sound2c: WARNING: no asset for %s (%s) - silent entry"
                  % (enum_name, java_name), file=sys.stderr)

    lines = []
    lines.append("/* GENERATED FILE - DO NOT EDIT.")
    lines.append(" * Produced by scripts/sound2c.py - sound effects packed into the binary.")
    lines.append(" * All samples normalized to mono / 16-bit signed / %d Hz." % TARGET_RATE)
    lines.append(" */")
    lines.append('#include "../sound/sound.h"')
    lines.append("")

    for enum_name, (arr, samples, rate, path, java_name) in sorted(entries.items()):
        lines.append("/* %s  (%s) */" % (java_name, os.path.basename(path)))
        lines.append("static const int16_t %s[%d] = {" % (arr, len(samples)))
        for i in range(0, len(samples), VALUES_PER_LINE):
            chunk = samples[i : i + VALUES_PER_LINE]
            lines.append("\t" + ",".join(str(v) for v in chunk) + ",")
        lines.append("};")
        lines.append("")

    lines.append("const SoundData sound_data_table[SOUND_COUNT] = {")
    for enum_name, (_arr, _s, _r, _p, _j) in sorted(entries.items()):
        arr, samples, rate = entries[enum_name][0], entries[enum_name][1], entries[enum_name][2]
        lines.append("\t[%s] = { %s, %du, %du }," % (enum_name, arr, len(samples), rate))
    lines.append("};")
    lines.append("")

    with open(out_path, "w", newline="\n") as f:
        f.write("\n".join(lines))

    print("sound2c: wrote %s (%d sounds)" % (out_path, len(entries)))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
