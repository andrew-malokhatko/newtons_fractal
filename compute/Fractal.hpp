#pragma once

// clang-format off
/// Computes fractal for polynomial like z^n - 1 = 0
/// @param n power of z
/// @param width image width in pixels
/// @param height image height in pixels
/// @param xmin minimum x (real part of viewport)
/// @param xmax maximum x (real part of viewport)
/// @param ymin minimum y (imaginary part of viewport)
/// @param ymax maximum y (imaginary part of viewport)
/// @param maxIter maximum iterations per pixel
/// @param tolerance maximum acceptable convergence
/// @param rootIdx Output buffer of size [width * height] with roots indices [0.. n-1]
/// @param itCounts Output buffer of size [width * height] filled with iteration counts

extern "C"
{

void computeNewtonFractal(
    int n, int width, int height,
    double xmin, double xmax,
    double ymin, double ymax,
    int maxIter,
    double tolerance,
    int* rootIdx,
    int* itCounts
);

}
// clang-format on