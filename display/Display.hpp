#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace disp
{

class Display
{
    using buffer_t = std::vector<Color>;

    size_t m_width;
    size_t m_height;
    buffer_t m_buffer;

    Image m_image{};
    Texture2D m_texture{};

    bool m_valid = true;

  public:
    Display(int width, int height, std::string windowName);
    void setFractal(buffer_t&& framebuffer);

    bool isValid() const;
    void update();
    void draw() const;
};

} // namespace disp
