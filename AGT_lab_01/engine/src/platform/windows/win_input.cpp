#include "pch.h"
#include "platform/windows/win_input.h"
#include "engine/core/application.h"
#include "GLFW/glfw3.h"

namespace engine
{
    // some how we'll have to delete this?
    input* input::s_instance = new win_input();

    static bool s_first_mouse = true;
}

bool engine::win_input::key_pressed_impl(int32_t key_code) const
{
    auto window = application::window().native_window();
    auto state = glfwGetKey(static_cast<GLFWwindow*>(window), key_code);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool engine::win_input::mouse_button_pressed_impl(int32_t button) const
{
    auto window = application::window().native_window();
    auto state = glfwGetMouseButton(static_cast<GLFWwindow*>(window), button);
    return state == GLFW_PRESS;
}

std::pair<float, float> engine::win_input::mouse_position_impl() const
{
    auto window = application::window().native_window();
    double x_pos, y_pos;
    glfwGetCursorPos(static_cast<GLFWwindow*>(window), &x_pos, &y_pos);

    // Always the absolute position. This used to return a delta when the
    // cursor was hidden and a position when it was visible, so callers could
    // not tell which they had been handed. The camera assumed "delta" either
    // way, and therefore added the absolute cursor position to its yaw on
    // every frame whenever the cursor was shown - the view spun. Movement
    // now comes from mouse_delta_impl() instead.
    return { static_cast<float>(x_pos), static_cast<float>(y_pos) };
}

std::pair<float, float> engine::win_input::mouse_delta_impl() const
{
    auto window = application::window().native_window();
    double x_pos, y_pos;
    glfwGetCursorPos(static_cast<GLFWwindow*>(window), &x_pos, &y_pos);

    const float x = static_cast<float>(x_pos);
    const float y = static_cast<float>(y_pos);

    // First call: report no movement. Differencing against an unset reference
    // would return the full cursor position and snap the camera round on the
    // opening frame.
    if (!s_has_last_position)
    {
        s_last_position = { x, y };
        s_has_last_position = true;
        return { 0.f, 0.f };
    }

    const float delta_x = x - s_last_position.first;
    // Flipped: window y grows downwards, but "up" should raise the pitch.
    const float delta_y = s_last_position.second - y;

    // Updated unconditionally. Doing this only while the cursor was hidden
    // left the reference stale across any period of cursor visibility, so
    // re-hiding it produced one huge jump.
    s_last_position = { x, y };

    return { delta_x, delta_y };
}

bool engine::win_input::cursor_captured_impl() const
{
    return !application::window().is_cursor_visible();
}

float engine::win_input::mouse_x_impl() const
{
    // c++ 17 way to assign std::pair(s)
    auto window = application::window().native_window();
    double x_pos, y_pos;
    glfwGetCursorPos(static_cast<GLFWwindow*>(window), &x_pos, &y_pos);
    return static_cast<float>(x_pos);
}

float engine::win_input::mouse_y_impl() const
{
    auto window = application::window().native_window();
    double x_pos, y_pos;
    glfwGetCursorPos(static_cast<GLFWwindow*>(window), &x_pos, &y_pos);
    return static_cast<float>(y_pos);
}
