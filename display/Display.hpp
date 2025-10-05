#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace disp
{

class Display
{
    using buffer_t = std::vector<Color>;
    static constexpr int FPS = 60;

    size_t m_width;
    size_t m_height;
    buffer_t m_buffer;

    Image m_image{};
    Texture2D m_texture{};

    bool m_valid = true;
    bool m_needsRedraw = true;

  public:
    Display(int width, int height, std::string windowName);

    void setImage(buffer_t&& framebuffer);
    bool isValid() const;
    void update();
    void draw();
};

} // namespace disp
