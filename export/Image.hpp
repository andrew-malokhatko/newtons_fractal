#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace img
{

void saveImageBMP(const std::string& filename, int width, int height, const std::vector<Color>& framebuffer);

} // namespace img