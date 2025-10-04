#include "Image.hpp"

#include <fstream>
#include <raylib.h>
#include <stdexcept>
#include <filesystem>

namespace img
{

void saveImageBMP(const std::string& filename, int width, int height, const std::vector<Color>& framebuffer)
{
    if (framebuffer.size() != width * height)
    {
        throw std::invalid_argument("Invalid framebuffer size");
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Could not create/open file \"" + filename + "\"");
    }

    // BMP header
    uint32_t fileSize = 14 + 40 + width * height * 4;
    uint32_t dataOffset = 14 + 40;

    unsigned char bmpFileHeader[14] = {
        'B', 'M',       // Signature
        0,   0,   0, 0, // File size
        0,   0,   0, 0, // Reserved
        0,   0,   0, 0  // Data offset
    };

    bmpFileHeader[2] = (unsigned char)(fileSize);
    bmpFileHeader[3] = (unsigned char)(fileSize >> 8);
    bmpFileHeader[4] = (unsigned char)(fileSize >> 16);
    bmpFileHeader[5] = (unsigned char)(fileSize >> 24);
    bmpFileHeader[10] = (unsigned char)(dataOffset);
    bmpFileHeader[11] = (unsigned char)(dataOffset >> 8);
    bmpFileHeader[12] = (unsigned char)(dataOffset >> 16);
    bmpFileHeader[13] = (unsigned char)(dataOffset >> 24);

    file.write(reinterpret_cast<char*>(bmpFileHeader), 14);

    // DIB header (BITMAPINFOHEADER)
    unsigned char dibHeader[40] = {0};
    dibHeader[0] = 40; // header size
    dibHeader[4] = (unsigned char)(width);
    dibHeader[5] = (unsigned char)(width >> 8);
    dibHeader[6] = (unsigned char)(width >> 16);
    dibHeader[7] = (unsigned char)(width >> 24);
    dibHeader[8] = (unsigned char)(height);
    dibHeader[9] = (unsigned char)(height >> 8);
    dibHeader[10] = (unsigned char)(height >> 16);
    dibHeader[11] = (unsigned char)(height >> 24);
    dibHeader[12] = 1;  // planes
    dibHeader[14] = 24; // bits per pixel
    // rest is zero (compression = 0, size image = 0, etc.)

    file.write(reinterpret_cast<char*>(dibHeader), 40);

    // Write pixels to buffer
    std::vector<unsigned char> pixels(width * height * 4);
    for (int y = 0; y < height; ++y)
    {
        int row = height - 1 - y; // BMP starts bottom-up

        for (int x = 0; x < width; ++x)
        {
            const Color& c = framebuffer[row * width + x];

            // bmp does not support alpha, so multiply all colors by it instead
            double alpha = c.a / 255.0;
            unsigned char r = static_cast<unsigned char>(c.r * alpha);
            unsigned char g = static_cast<unsigned char>(c.g * alpha);
            unsigned char b = static_cast<unsigned char>(c.b * alpha);

            int idx = (y * width + x) * 3;
            pixels[idx + 0] = b;
            pixels[idx + 1] = g;
            pixels[idx + 2] = r;
        }
    }

    file.write(reinterpret_cast<char*>(pixels.data()), pixels.size());
}

} // namespace exp