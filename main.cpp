#include "compute/Fractal.hpp"
#include "display/Display.hpp"
#include "export/Image.hpp"

#include <array>
#include <complex>
#include <iostream>
#include <raylib.h>
#include <stdexcept>

#define COLORS_SIZE 12
static constexpr std::array<Color, COLORS_SIZE> Colors{
    Color{247, 45, 89, 255},  // Red/Pink
    Color{45, 247, 89, 255},  // Green
    Color{140, 15, 226, 255}, // Purple
    Color{188, 188, 5, 255},  // Yellow
    Color{15, 140, 226, 255}, // Blue
    Color{255, 147, 0, 255},  // Orange
    Color{0, 255, 255, 255},  // Cyan
    Color{255, 0, 255, 255},  // Magenta
    Color{191, 255, 0, 255},  // Lime
    Color{0, 128, 128, 255},  // Teal
    Color{128, 0, 255, 255},  // Violet
    Color{255, 99, 71, 255}   // Coral
};

struct ViewPort
{
    double minx{};
    double maxx{};
    double miny{};
    double maxy{};
};

struct Config
{
    int n = 5;
    int width = 1000;
    int height = 1000;
    ViewPort viewport = {-2.0, 2.0, -2.0, 2.0};
    int maxIterations = 40;
    double tolerance = 1e-4;

    bool write = false;
    std::string writePath = "";
};

Config parseArgs(int argc, char* argv[])
{
    Config conf;

    for (int i = 1; i < argc; ++i)
    {
        if (i + 1 >= argc)
        {
            throw std::invalid_argument("Not enough arguments");
        }

        std::string arg = argv[i];
        std::string argValue = argv[++i];

        if (arg == "-n")    // number of roots
        {
            conf.n = std::stoi(argValue);
        }
        else if (arg == "-s")   // size
        {
            int size = std::stoi(argValue);
            conf.width = size;
            conf.height = size;
        }
        else if (arg == "-v")   // viewport
        {
            double viewport = std::stod(argValue);
            conf.viewport.minx = -viewport;
            conf.viewport.maxx = viewport;
            conf.viewport.miny = -viewport;
            conf.viewport.maxy = viewport;
        }
        else if (arg == "-i")    // max iterations
        {
            conf.maxIterations = std::stoi(argValue);
        }
        else if (arg == "-t")   // tolerance
        {
            conf.tolerance = std::stod(argValue);
        }
        else if (arg == "-w")
        {
            conf.write = true;
            conf.writePath = argValue;
        }
        else
        {
            throw std::invalid_argument("Unknown argument " + arg);
        }
    }

    return conf;
}

int main(int argc, char* argv[])
{
    // parse input
    Config conf{};
    try
    {
        conf = parseArgs(argc, argv);
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << e.what() << "\n";
        std::exit(1);
    }

    // allocate place for result of computeNewtonFractal()
    std::vector<int> rootIndices(conf.width * conf.height);
    std::vector<int> iterationCounts(conf.width * conf.height);
    std::vector<Color> framebuffer(conf.width * conf.height);

    // clang-format off
    computeNewtonFractal(conf.n, conf.width, conf.height,
    conf.viewport.minx, conf.viewport.maxx, conf.viewport.miny, conf.viewport.maxy,
    conf.maxIterations,
        conf.tolerance,
        rootIndices.data(),
        iterationCounts.data()
    );
    // clang-format on

    // fill in framebuffer for Display
    for (int i = 0; i < conf.width * conf.height; ++i)
    {
        int root = rootIndices[i];
        int iteration = iterationCounts[i];

        auto pixelColor = BLACK;
        if (root != -1)
        {
            pixelColor = Colors[root % COLORS_SIZE];
        }

        // change color alpha based on iterations
        pixelColor.a = static_cast<unsigned char>(255.0 - ((255.0 / conf.maxIterations) * iteration));

        framebuffer[i] = pixelColor;
    }
    if (conf.write)
    {
        try
        {
            img::saveImageBMP(conf.writePath, conf.width, conf.height, framebuffer);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << "\n";
            std::exit(2);
        }
    }
    else
    {
        disp::Display display(conf.width, conf.height, "Newton's Fractal");
        display.setFractal(std::move(framebuffer));

        // draw window until it's closed
        while (display.isValid())
        {
            display.update();
            display.draw();
        }
    }
}