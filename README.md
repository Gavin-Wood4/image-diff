# Image Differencing Tool

A command-line tool written in C to calculate the pixel by pixel difference of color between two RGBA image files. 

This project demonstrates C programming fundamentals, memory management, command line argument parsing, and performance optimization using NEON for ARM64 architectures. 

## Features

- **Standard/Portable C Executable (`diff`):** A portable version written using standard C11.
- **NEON-Optimized Executable (`neon-diff`):** An ARM64-specific version using NEON intrinsics. (Work in progress. Only modular mode is working.)
- **Difference Modes:**
    - **[Default] Absolute (`abs`):** `|img1 - img2|`
    - **Saturated (`sat`):** `max(0, img1 - img2)`
    - **Modular (`mod`):** `(img1 - img2) % 256`
- **RGBA IO:** Reads and writes RGBA images. 
- **Native or Cross-compiler Build:** `Makefile` supporting native builds with architecture detection (x86_64/aarch64) and cross-compilation for Raspberry Pi 5 (Cortex-A76).
- **C Standard Compliance:** Built with `-O3 -Wall -Wextra -pedantic` for performance and strict C11 compliance.

## Building

The project can be built using the provided `Makefile`.

```bash
# Clean previous builds
make clean

# Build for native host (auto-detects aarch64 for neon-diff)
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

# Example using modular difference (short name)
./diff image1.rgba image2.rgba output_mod.rgba mod

# Example using neon-diff (WIP - only modular mode)
./neon-diff image1.rgba image2.rgba output_neon_mod.rgba
```

If running cross-compiled neon-diff on x86_64, and received an error:
`-bash: ./neon-diff: cannot execute binary file: Exec format error`

Try installing `qemu-aarch64-static` and run using:
`qemu-aarch64-static ./neon-diff image1.rgba image2.rgba output_neon_mod.rgba`

## Project Status

- `diff.c`: Functionally complete and tested with all modes.
- `neon-diff.c` Work in progress. Parsing for mode selection is complete, however it has no functionally complete mode selection and only does modular subtraction of pixels. 
- `rgba_io`: Functionally complete. 
- No script to automagically test functionality of executables. 
- No script to test performance of each executable and compare. 

## To-Do

- [ ] Add and test absolute, saturation, and modular subtraction modes in neon-diff.c
- [ ] Create performance benchmark scripts and document comparisons between `diff.c` and `neon-diff.c`
- [ ] Unit tests for all functions in each program. 
	- [ ] rgba_io
	- [ ] diff.c
	- [ ] neon-diff.c
- [ ] Add support for common image formats (PNG/JPG)
