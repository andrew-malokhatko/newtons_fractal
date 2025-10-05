#include "Display.hpp"

#include <cassert>
#include <cstring>

namespace disp
{

Display::Display(int width, int height, std::string windowName)
    : m_width(width),
      m_height(height),
      m_buffer(width * height, {0, 0, 0, 255})
{
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(width, height, windowName.c_str());
    SetTargetFPS(FPS);

    m_image = GenImageColor(width, height, BLANK);
    m_texture = LoadTextureFromImage(m_image);
}

void Display::setImage(buffer_t&& framebuffer)
{
    assert(framebuffer.size() == m_buffer.size());
    m_buffer = std::move(framebuffer);

    std::memcpy(m_image.data, m_buffer.data(), m_buffer.size() * sizeof(Color));
    UpdateTexture(m_texture, m_image.data);

    m_needsRedraw = true;
}

bool Display::isValid() const
{
    return m_valid;
}

void Display::update()
{
    if (WindowShouldClose())
    {
        m_valid = false;
        CloseWindow();
    }
}

void Display::draw()
{
    if (!m_valid)
    {
        return;
    }

    if (!m_needsRedraw)
    {
        BeginDrawing(); // wait in raylib to reduce CPU usage
        EndDrawing();
        return;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(m_texture, 0, 0, WHITE);
    EndDrawing();

    m_needsRedraw = false;
}

} // namespace disp
