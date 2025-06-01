# Image Differencing Tool

A command-line tool written in C to calculate the pixel by pixel difference of color between two images. 

Current supported Extensions are `rgba` and `.png`.

This project demonstrates C programming fundamentals, memory management, command line argument parsing, and performance optimization using NEON intrinsics for ARM64 architectures. 

## Features

- **Standard/Portable C Executable (`diff`):** A portable version written using standard C11.
- **NEON-Optimized Executable (`neon-diff`):** An ARM64-specific version using NEON intrinsics. 
- **Difference Modes:**
    - **[Default] Absolute (`abs`):** `|img1 - img2|`
    - **Saturated (`sat`):** `max(0, img1 - img2)`
    - **Modular (`mod`):** `(img1 - img2) % 256`
- **RGBA IO:** Reads and writes RGBA and PNG images.
- **Native or Cross-compiler Build:** `Makefile` supporting native builds with architecture detection (x86_64/aarch64) and cross-compilation for Raspberry Pi 5 (Cortex-A76).
- **C Standard Compliance:** Built with `-O3 -Wall -Wextra -pedantic` for performance and strict C11 compliance.

## Building

The project can be built using the provided `Makefile`.

```bash
# Clean previous builds
make clean

# Build for native host (auto-detects aarch64 for `neon-diff`)
make

# Force a HOST-only build (only builds 'diff')
make HOST=1

# Cross-compile for Raspberry Pi 5 (requires aarch64-linux-gnu-gcc)
make PI=1
```

## Usage

The two executables `diff` and `neon-diff` can be executed from the command line. 
```bash
# Example using absolute difference (default)
./diff image1.rgba image2.rgba output_abs.rgba

# Example using saturated difference (long name)
./diff image1.rgba image2.rgba output_sat.rgba saturated

# Example using modular difference (short name & png output)
./diff image1.rgba image2.rgba output_mod.png mod

# Example using neon-diff
./neon-diff image1.rgba image2.rgba output_neon_mod.png mod

# Example using neon-diff with PNG input
./neon-diff image1.png image2.png output_neon_sat.rgba sat

# Example using neon-diff with mixed file type
./neon-diff image1.rgba image2.png output_neon_abs.rgba
```

If running cross-compiled neon-diff on x86_64, and received an error:
`-bash: ./neon-diff: cannot execute binary file: Exec format error`

Try installing `qemu-aarch64-static` and run using:
`qemu-aarch64-static ./neon-diff image1.rgba image2.rgba output_neon_mod.rgba`

## Project Status

- `diff.c`: Functionally complete and tested with all modes.
- `neon-diff.c`: Functionally complete and tested with all modes. 
- `rgba_io`: Functionally complete. Supports input and output of PNG and RGBA files.  
- No script to test functionality and performance of each executable and compare. 

## To-Do

- [X] Add and test absolute, saturation, and modular subtraction modes in neon-diff.c
- [ ] Create performance benchmark scripts and document comparisons between `diff.c` and `neon-diff.c`
- [ ] Unit tests for all functions in each program. 
	- [ ] rgba_io
	- [ ] diff.c
	- [ ] neon-diff.c
    - [ ] pix_diff
- [X] Add support for common image formats (PNG/JPG)
    - [X] Input
    - [X] Output

## Third-Party Libraries

This project uses a third-party library:

### stb_image.h

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
