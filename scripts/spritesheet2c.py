#!/usr/bin/env python3
"""
spritesheet2c.py - Convert a spritesheet PNG into C arrays (Minicraft format).

Replicates EXACTLY what the original Java code does (SpriteSheet.java):

    pixels[i] = (pixels[i] & 0xff) / 64;     // blue channel / 64  ->  0..3

Each output pixel is a 2-bit index used by Screen.render() to pick one of the
four 8-bit color channels of the sprite "colors" int.

Pure Python stdlib PNG decoder (zlib + struct), no Pillow required, so it
works on any machine / CI / cross environment that has Python 3.
Supports non-interlaced PNGs with bit depth 8 and color types:
    0 (gray), 2 (RGB), 3 (palette), 4 (gray+alpha), 6 (RGBA)

Usage:
    python3 scripts/spritesheet2c.py INPUT.png OUTPUT_BASENAME [SYMBOL_PREFIX]

Generates OUTPUT_BASENAME.c and OUTPUT_BASENAME.h containing:
    const SpriteSheet <prefix>_spritesheet = { w, h, <prefix>_pixels };
    extern const SpriteSheet <prefix>_spritesheet;   (in the .h)

Example:
    python3 scripts/spritesheet2c.py assets/icons.png source/extern/icons_data icons
"""

import os
import struct
import sys
import zlib

PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"

# Number of values per line in the generated C array
VALUES_PER_LINE = 32


def read_chunks(data):
    """Yield (type, payload) tuples from raw PNG file bytes."""
    if not data.startswith(PNG_SIGNATURE):
        raise ValueError("Not a PNG file")
    pos = len(PNG_SIGNATURE)
    while pos < len(data):
        (length,) = struct.unpack(">I", data[pos : pos + 4])
        ctype = data[pos + 4 : pos + 8]
        payload = data[pos + 8 : pos + 8 + length]
        (crc,) = struct.unpack(">I", data[pos + 8 + length : pos + 12 + length])
        if zlib.crc32(ctype + payload) & 0xFFFFFFFF != crc:
            raise ValueError("Bad CRC in chunk %r" % ctype)
        yield ctype, payload
        pos += 12 + length


def paeth_predictor(a, b, c):
    p = a + b - c
    pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
    if pa <= pb and pa <= pc:
        return a
    if pb <= pc:
        return b
    return c


def unfilter(raw, width, height, bpp):
    """Reverse PNG scanline filtering. Returns bytearray of RGB-ish rows."""
    stride = width * bpp
    out = bytearray(height * stride)
    pos = 0
    prev_row = bytearray(stride)  # conceptually all zeros above the first row
    for y in range(height):
        ftype = raw[pos]
        pos += 1
        line = bytearray(raw[pos : pos + stride])
        pos += stride
        if ftype == 0:  # None
            pass
        elif ftype == 1:  # Sub
            for i in range(bpp, stride):
                line[i] = (line[i] + line[i - bpp]) & 0xFF
        elif ftype == 2:  # Up
            for i in range(stride):
                line[i] = (line[i] + prev_row[i]) & 0xFF
        elif ftype == 3:  # Average
            for i in range(stride):
                a = line[i - bpp] if i >= bpp else 0
                line[i] = (line[i] + ((a + prev_row[i]) >> 1)) & 0xFF
        elif ftype == 4:  # Paeth
            for i in range(stride):
                a = line[i - bpp] if i >= bpp else 0
                c = prev_row[i - bpp] if i >= bpp else 0
                line[i] = (line[i] + paeth_predictor(a, prev_row[i], c)) & 0xFF
        else:
            raise ValueError("Unknown filter type %d" % ftype)
        out[y * stride : (y + 1) * stride] = line
        prev_row = line
    return out


def decode_png(data):
    """Decode a PNG. Returns (width, height, list_of_(r,g,b) tuples)."""
    width = height = None
    bit_depth = color_type = interlace = None
    palette = None
    idat = bytearray()

    for ctype, payload in read_chunks(data):
        if ctype == b"IHDR":
            (width, height, bit_depth, color_type,
             _comp, _filt, interlace) = struct.unpack(">IIBBBBB", payload)
        elif ctype == b"PLTE":
            palette = payload
        elif ctype == b"IDAT":
            idat += payload
        elif ctype == b"IEND":
            break

    if width is None:
        raise ValueError("Missing IHDR chunk")
    if bit_depth != 8:
        raise ValueError("Only 8-bit PNGs supported (got %d)" % bit_depth)
    if interlace != 0:
        raise ValueError("Interlaced PNGs are not supported")

    channels = {0: 1, 2: 3, 3: 1, 4: 2, 6: 4}.get(color_type)
    if channels is None:
        raise ValueError("Unknown color type %d" % color_type)
    if color_type == 3 and palette is None:
        raise ValueError("Paletted PNG without PLTE chunk")

    raw = zlib.decompress(bytes(idat))
    flat = unfilter(raw, width, height, channels)

    pixels = []
    for i in range(width * height):
        if color_type == 3:  # palette index
            idx = flat[i] * 3
            pixels.append((palette[idx], palette[idx + 1], palette[idx + 2]))
        elif color_type == 2:  # RGB
            o = i * 3
            pixels.append((flat[o], flat[o + 1], flat[o + 2]))
        elif color_type == 6:  # RGBA
            o = i * 4
            pixels.append((flat[o], flat[o + 1], flat[o + 2]))
        elif color_type in (0, 4):  # gray / gray+alpha  (R=G=B=gray in Java getRGB)
            o = i * channels
            pixels.append((flat[o], flat[o], flat[o]))
    return width, height, pixels


def to_minicraft_values(pixels):
    """Apply the original Java formula: (argb & 0xff) / 64  →  blue / 64."""
    return [(b & 0xFF) // 64 for (_r, _g, b) in pixels]


def generate_c(png_path, out_base, prefix):
    with open(png_path, "rb") as f:
        data = f.read()
    width, height, pixels = decode_png(data)
    values = to_minicraft_values(pixels)

    lines = []
    for i in range(0, len(values), VALUES_PER_LINE):
        chunk = values[i : i + VALUES_PER_LINE]
        lines.append("\t" + ",".join(str(v) for v in chunk) + ",")
    if lines:
        lines[-1] = lines[-1].rstrip(",")  # drop trailing comma (valid C99 anyway)

    guard = ("GENERATED_%s_H_" % prefix).upper()

    header = (
        "/* GENERATED FILE - DO NOT EDIT.\n"
        " * Produced by scripts/spritesheet2c.py from: %s\n"
        " * Formula (same as original Java SpriteSheet): value = blue_channel / 64\n"
        " */\n"
        "#ifndef %s\n"
        "#define %s\n"
        "\n"
        '#include "../gfx/spritesheet.h"\n'
        "\n"
        "extern const SpriteSheet %s_spritesheet;\n"
        "\n"
        "#endif /* %s */\n"
        % (png_path, guard, guard, prefix, guard)
    )

    source = (
        "/* GENERATED FILE - DO NOT EDIT.\n"
        " * Produced by scripts/spritesheet2c.py from: %s (%dx%d)\n"
        " * Formula (same as original Java SpriteSheet): value = blue_channel / 64\n"
        " */\n"
        '#include "%s.h"\n'
        "\n"
        "static const unsigned char %s_pixels[%d] = {\n"
        "%s\n"
        "};\n"
        "\n"
        "const SpriteSheet %s_spritesheet = {\n"
        "\t%d,\n"
        "\t%d,\n"
        "\t%s_pixels\n"
        "};\n"
        % (png_path, width, height, os.path.basename(out_base), prefix, len(values),
           "\n".join(lines), prefix, width, height, prefix)
    )

    with open(out_base + ".c", "w", newline="\n") as f:
        f.write(source)
    with open(out_base + ".h", "w", newline="\n") as f:
        f.write(header)

    print("spritesheet2c: %s -> %s.c/.h (%dx%d, %d px)"
          % (png_path, out_base, width, height, len(values)))


def main(argv):
    if len(argv) != 4:
        print(__doc__)
        return 1
    generate_c(argv[1], argv[2], argv[3])
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
