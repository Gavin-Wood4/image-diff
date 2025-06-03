
# Image Differencing Tool

A command-line tool written in C to calculate the pixel by pixel difference of color between two images. 

Current supported input/output extensions are `rgba` and `.png`.

This project demonstrates C programming fundamentals, memory management, command line argument parsing, and performance optimization using NEON intrinsics for ARM64 architectures. 

## Features

- **Portable C Executable (`diff`):** A portable version written using standard C11. Compiles `diff_neon()` conditional on presence of `__ARM_NEON` preprocessor macro definition.
- **Difference Modes:**
    - **[Default] Absolute (`abs`):** `|img1 - img2|`
    - **Saturated (`sat`):** `max(0, img1 - img2)`
    - **Modular (`mod`):** `(img1 - img2) % 256`
- **Disable Neon Difference Flag:**
	- **disable_neon:** Forces scalar differencing if last argument passed is `disable_neon`
- **Image IO:** Reads and writes RGBA and PNG images.
- **Pix Diff:** Calculates and returns the image difference data buffer from the passed `img1` and `img2` and `size`.
- **Native or Cross-compiler Build:** `Makefile` supports native builds with architecture detection (x86_64/aarch64) and cross-compilation for Raspberry Pi 5 (Cortex-A76).
- **C Standard Compliance:** Built with `-O3 -Wall -Wextra -pedantic` for performance and strict C11 compliance.

## Building

The project can be built using the provided `Makefile`.

```bash
# Clean previous builds
make clean

# Build for native host (auto-detects aarch64 or other)
make

# Force a HOST-only build (only builds based on native preprocessor macros)
make HOST=1

# Cross-compile for Raspberry Pi 5 (requires aarch64-linux-gnu-gcc)
make PI=1
```

## Usage

The executable `diff` can be executed from the command line. 
```bash
# Example using absolute difference (default)
./diff image1.rgba image2.rgba output_abs.rgba

# Example using saturated difference (long name)
./diff image1.rgba image2.rgba output_sat.rgba saturated

# Example using modular difference (short name & png output)
./diff image1.rgba image2.rgba output_mod.png mod

# Example using disable_neon flag and mixed extension input
./diff image1.png image2.rgba output_abs_scalar.png disable_neon

# Example using saturated difference, disable_neon flag, and mixed extension output
./diff image1.png image2.png output_sat_scalar.rgba sat disable_neon
```

If running cross-compiled `diff` for aarch64 using `make PI=1` on x86_64, and received an error:
`-bash: ./diff: cannot execute binary file: Exec format error`

Try installing `qemu-aarch64-static` and run using:
`qemu-aarch64-static ./diff image1.png image2.png output.png`

## Project Status

- `diff.c`: Functionally complete and tested with all modes.
- `image_io`: Functionally complete. Supports input and output of PNG and RGBA files. May add JPG input and output and some other common types (BMP).
- `pix_diff`: Functionally complete. Supports scalar based or manually vectorized subtraction of pixels. 
- No script to test functionality and performance of each executable and compare. 

## To-Do

- [X] Add and test absolute, saturation, and modular subtraction modes in neon-diff.c
- [ ] Create performance benchmark scripts and document comparisons between neon and scalar pixel differencing functions.
- [ ] Unit tests for all functions in each program. 
	- [ ] image_io
	- [ ] diff.c
    - [ ] pix_diff
- [X] Add support for common image formats
    - [X] RGBA
    - [X] PNG
    - [ ] JPG
    - [ ] BMP
    - [ ] Maybe SVGs somehow?
    - [ ] GIFs would be mega cool, frame by frame. May be better to make this a library and call for that.
- [ ] Change core logic to be a single linkable library (?)

## Third-Party Libraries

This project uses a third-party library:

### stb_image.h & stb_image_write.h

-   **Author:** Sean Barrett
-   **Repository:** [https://github.com/nothings/stb](https://github.com/nothings/stb)
-   **License:** Public Domain or MIT License:

```text
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
