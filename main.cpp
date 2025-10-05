#include "compute/Fractal.hpp"
#include "display/Display.hpp"
#include "export/Image.hpp"

#include <array>
#include <complex>
#include <cstring>
#include <iostream>
#include <raylib.h>
#include <stdexcept>

static constexpr size_t COLORS_SIZE = 12;
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
    int n = 5;          // number of roots
    int width = 1000;   // image width
    int height = 1000;  // image height
    ViewPort viewport = {-2.0, 2.0, -2.0, 2.0}; // visible complex plane
    int maxIterations = 40;     // max iterations per pixel
    double tolerance = 1e-4;    // convergence tolerance (how close must be to a root)

    bool write = false;     // write image to writePath
    std::string writePath = ""; // name of the saved image
};

void printHelp()
{
    std::cout << "Usage: ./newtons_fractal [options]\n\n"
              << "Options:\n"
              << "  -n <int>        Power of the polynomial (solves z^n - 1 = 0).\n"
              << "  -s <int>        Image size in pixels (sets both width and height).\n"
              << "  -v <double>     Viewport size (visible area of the complex plane).\n"
              << "  -i <int>        Maximum iterations per pixel for Newton's method.\n"
              << "  -t <double>     Convergence tolerance (distance threshold to a root).\n"
              << "  -w <filename>   Write output image to a file (.bmp format) instead of displaying it.\n"
              << "  -h, --help      Show this help message and exit.\n\n"
              << "Examples:\n"
              << "  ./newtons_fractal -n 3 -s 800 -v 2.5 -i 50 -t 1e-6\n"
              << "  ./newtons_fractal -n 5 -s 1024 -w output.bmp\n";
}

// throws std::invalid_argument and std::out_of_range
// can exit if --help or -h is passed as an argument
Config parseArgs(int argc, char* argv[])
{
    Config conf{};

    // print help
    if (argc >= 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        printHelp();
        std::exit(0);
    }

    for (int i = 1; i < argc; i += 2)
    {
        if (i + 1 >= argc)
        {
            throw std::invalid_argument("Not enough arguments");
        }

        std::string arg = argv[i];
        std::string argValue = argv[i + 1];

        if (arg == "-n") // number of roots
        {
            conf.n = std::stoi(argValue);
            if (conf.n < 1)
            {
                throw std::invalid_argument("Invalid number of roots: " + argValue);
            }
        }
        else if (arg == "-s") // size
        {
            int size = std::stoi(argValue);
            if (size <= 0)
            {
                throw std::invalid_argument("Invalid size: " + argValue);
            }

            conf.width = size;
            conf.height = size;
        }
        else if (arg == "-v") // viewport
        {
            double viewport = std::stod(argValue);
            conf.viewport.minx = -viewport;
            conf.viewport.maxx = viewport;
            conf.viewport.miny = -viewport;
            conf.viewport.maxy = viewport;
        }
        else if (arg == "-i") // max iterations
        {
            conf.maxIterations = std::stoi(argValue);
            if (conf.maxIterations <= 0)
            {
                throw std::invalid_argument("Invalid iterations: " + argValue);
            }
        }
        else if (arg == "-t") // tolerance
        {
            conf.tolerance = std::stod(argValue);
            if (conf.tolerance <= 0)
            {
                throw std::invalid_argument("Invalid tolerance: " + argValue);
            }
        }
        else if (arg == "-w")   // write
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
    Config conf;
    try
    {
        conf = parseArgs(argc, argv);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        printHelp();
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
        display.setImage(std::move(framebuffer));

        // draw window until it's closed
        while (display.isValid())
        {
            display.update();
            display.draw();
        }
    }
}