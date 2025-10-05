# Overview
This program is used to generate [Newtons Fractal](https://en.wikipedia.org/wiki/Newton_fractal),
visual representations of the convergence behavior of Newton's method applied to complex polynomials.

The core computation of Newton's method is implemented in two languages:

- **C++**
- **ISPC**

This allows the program not only to produce fractal images, but also serve as
a benchmark for comparing performance of ISCP against C++ implementation.

# Dependencies / Requirements

To build `newtons_fractal` you need:
- **[raylib](https://www.raylib.com/)** — for graphics.
  *(If raylib is not found on your system, it will be automatically downloaded using CMake’s `FetchContent`.)*
- **[ISPC Compiler](https://ispc.github.io/)** — to build the ISPC implementation.
- **[GoogleTest (GTest)](https://github.com/google/googletest)** — optional, required only if tests are enabled (`-DBUILD_TESTS=ON`).

# Build & Run

To build the program, follow these steps:
```bash
git clone https://github.com/andrew-malokhatko/newtons_fractal
cd newtons_fractal
mkdir build
cd build
cmake ..
cmake -- build .
```

### Build Options
The project provides two optional build flags:
- `USE_ISPC` - Enable the ISPC implementation of Newton's Fractal (default: `ON`)
- `BUILD_TESTS` - Build performance tests (default: `OFF`)

For example, to build tests and disable ISPC, configure `CMake` with:
```bash
cmake .. -DBUILD_TESTS=ON -DUSE_ISPC=OFF
```

# Usage
You can run the program without any arguments:
```bash
./newtons_fractal
```

This generates a default Newton’s fractal image using the configuration from `Config`.

### Command-Line Options
```bash
./newtons_fractal [options]
```
- `-n <int>` - power of the polynomial (z<sup>n</sup> - 1 = 0).
- `-s <int>` - image size in pixels (sets both width and height).
- `-v <double>` - viewport size, visible complex plane.
- `-i <int>` - maximum iterations per pixel with Newton's method.
- `-t <double>` - convergence tolerance (how close a point must be to a root)
- `-w <filenamee>` - write the image to a file (.bmp format) instead of displaying it.
- `-h, --help` - Show help message and exit.

### Examples
Generate and save 3-root fractal to a file:
```bash
./newtons_fractal -n 3 -w image.bmp
```

Render fractal directly in window (viewport.x and viewport.y from -4 to 4, max 40 iterations):
```bash
./newtons_fractal -v 4 -i 40
```

# Tests
This project includes performance tests using **[GoogleTest (GTest)](https://github.com/google/googletest)** to
compare the execution speed of the C++ and ISPC implementation.

On my machine the ISPC ran over **16x faster**:
- **C++**: ~50 seconds
- **ISPC**: ~3 seconds

### Running Tests
From the `build` directory run:
```bash
ctest --verbose
```
The `--verbose` flag ensures that timing information is displayed.