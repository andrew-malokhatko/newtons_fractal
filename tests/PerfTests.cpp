#include "../compute/Fractal.hpp"
#include <gtest/gtest.h>

/// Functional tests for Debugger class
class PerfTests : public ::testing::Test
{
};

TEST_F(PerfTests, BasePerf)
{
    const int Width = 5'000;
    const int Height = 5'000;
    const int MaxIters = 100;
    const int TestIters = 3;
    std::vector<int> rootIndices(Width * Height);
    std::vector<int> iterationCounts(Width * Height);

    #ifdef USE_ISPC
        std::string method = "ISPC";
    #else
        std::string method = "C++";
    #endif

    std::cout << "Testing computation speed with " << method << "\n";
    std::cout << "Width: " << Width << "\n";
    std::cout << "Height: " << Height << "\n";
    std::cout << "Iterations: " << MaxIters << "\n";

    double bestTime = DBL_MAX;
    for (int i = 0; i < TestIters; ++i)
    {
        auto start = std::chrono::high_resolution_clock::now();
        // clang-format off
        computeNewtonFractal(
            5, Width, Height,
            -2.0, 2.0, -2.0, 2.0,   // viewport
            MaxIters,
            1e-6,
            rootIndices.data(),
            iterationCounts.data()
        );
        // clang-format on
        auto finish = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::milli>(finish - start).count();

        bestTime = std::min(bestTime, time);
    }

    std::cout << "Ran computeNewtonFractal() for " << TestIters << " times\n";
    std::cout << "Best time: " << bestTime << "\n";
}