#include "Fractal.hpp"

#include <cfloat>
#include <complex>
#include <vector>

using complex_t = std::complex<double>;

/// get value of a polynomial like z^n - 1 = 0
/// @param n power of z
/// @param z value of z
complex_t getNormal(int n, complex_t z)
{
    return std::pow(z, n) - complex_t{1, 0};
}

/// get derivative of a polynomial like z^n - 1 = 0.
/// @param n power of z
/// @param z value of z
complex_t getDerivative(int n, complex_t z)
{
    return complex_t{static_cast<double>(n), 0} * std::pow(z, n - 1);
}

std::vector<complex_t> findRoots(int n)
{
    std::vector<complex_t> roots{};
    roots.reserve(n);
    double pi = std::numbers::pi;
    for (int k = 0; k < n; ++k)
    {
        double angle = 2.0 * pi * k / n;
        roots.emplace_back(std::cos(angle), std::sin(angle));
    }
    return roots;
}

extern "C"
{

// clang-format off
void computeNewtonFractal(
    int n, int width, int height,
    double xmin, double xmax,
    double ymin, double ymax,
    int maxIter,
    double tolerance,
    int* rootIdx,
    int* itCounts
)
// clang-format on
{
    double xstep = (xmax - xmin) / width;
    double ystep = (ymax - ymin) / height;

    auto roots = findRoots(n);

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            double curx = xmin + (col * xstep);
            double cury = ymin + (row * ystep);

            complex_t point{curx, cury};
            int iter = 0;

            // find the nearest root using Euler's method
            while (iter < maxIter)
            {
                auto normal = getNormal(n, point);
                auto derivative = getDerivative(n, point);

                if (std::abs(normal) < tolerance || std::abs(derivative) < tolerance)
                {
                    break;
                }

                auto step = normal / derivative;
                point -= step;
                ++iter;
            }

            // determine root index
            int rootIndex = -1; // default: no root
            double minDist = DBL_MAX;
            for (int i = 0; i < n; ++i)
            {
                double rootDist = abs(point - roots[i]);
                if (rootDist > tolerance)
                {
                    continue;
                }
                if (rootDist < minDist)
                {
                    minDist = rootDist;
                    rootIndex = i;
                }
            }

            int index = row * width + col;
            rootIdx[index] = rootIndex;
            itCounts[index] = iter;
        }
    }
}

}