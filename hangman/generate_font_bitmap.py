import os

try:
    from PIL import Image, ImageFont, ImageDraw
except ImportError:
    print("Pillow library is required.")
    exit(1)


def generate_bitmap(char, font):
    # 1. Draw huge canvas to capture full char
    canvas_width = 64
    canvas_height = 64
    img = Image.new("L", (canvas_width, canvas_height), 0)
    draw = ImageDraw.Draw(img)

    # Draw char roughly in center
    draw.text((10, 10), char, font=font, fill=255)

    # 2. Get bounding box of actual pixels
    bbox = img.getbbox()
    if not bbox:
        # Space or empty char
        return 4, [0] * (2 * 16)  # Fallback

    # 3. Crop to the visible pixels
    cropped = img.crop(bbox)
    w, h = cropped.size

    # 4. Resize to height 16px (Anamorphic: Keep Width, Squash Height)
    #    User wants wide characters (e.g. A=17) but 16px height.
    #    If we generated size 22, height is likely > 16.
    target_height = 16
    # We maintain the ORIGINAL width 'w' to match the user's "17, 15..." expectation
    # even if it looks squashed.
    resized = cropped.resize((w, target_height), Image.Resampling.LANCZOS)

    final_width = w + 2  # Add padding
    final_img = Image.new("L", (final_width, target_height), 0)

    # Paste centered horizontally (1px padding)
    final_img.paste(resized, (1, 0))

    # 5. Generate Bitmap Data (Page Mode)
    bytes_list = []

    for page in range(2):
        for col in range(final_width):
            byte_val = 0
            for bit in range(8):
                y = page * 8 + bit
                # Check bounds
                if col < final_width and y < target_height:
                    pixel = final_img.getpixel((col, y))
                    if pixel > 128:
                        byte_val |= 1 << bit
            bytes_list.append(byte_val)

    return final_width, bytes_list


def main():
    # Attempt to load font - User prefers widths matching Arial/Sans style
    font_paths = [
        "C:/Windows/Fonts/arial.ttf",
        "arial.ttf",
        "C:/Users/lota/Downloads/neodgm.ttf",
    ]

    font = None
    for path in font_paths:
        if os.path.exists(path):
            try:
                # Use size 22 to get widths like 17px
                font = ImageFont.truetype(path, 22)
                break
            except Exception:
                continue

    if font is None:
        print("// Error: Could not load any font.")
        return

    chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

    print("#ifndef BITMAP_ALPHABET_H")
    print("#define BITMAP_ALPHABET_H")
    print("")
    print("#include <avr/pgmspace.h>")
    print("")

    # Existing numbers (optional to keep? User wants A-Z)
    # But usually we want everything in one place.
    # For now, let's just generate A-Z as requested.

    addr_table = []
    width_table = []

    for i, char in enumerate(chars):
        w, bitmap = generate_bitmap(char, font)

        var_name = f"bitmap_{ord(char)}"  # e.g. bitmap_65 for 'A'

        # Format hex output
        hex_data = ", ".join([f"0x{b:02X}" for b in bitmap])

        print(f"// Character '{char}' ({ord(char)})")
        print(f"const char {var_name}[] PROGMEM = {{{hex_data}}};")

        addr_table.append(var_name)
        width_table.append(w)

    print("")

    # Generate Helper Tables
    # We need to be careful. The user's code expects `char_addr[index]` logic.
    # If we only have A-Z here, the indexing must match the user's logic (c - 'A').

    print("// Width Table")
    print("const uint8_t alphabet_width[] = {")
    print(", ".join(map(str, width_table)))
    print("};")
    print("")

    print("// Address Table")
    print("const char * const alphabet_addr[] = {")
    print(", ".join(addr_table))
    print("};")

    print("")
    print("#endif // BITMAP_ALPHABET_H")


if __name__ == "__main__":
    main()
