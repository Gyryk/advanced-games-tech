#pragma once

#include "engine/core.h"

namespace engine
{
    /// \brief Base class for input polling.
    /// input polling allows to retrieve input states at any time.
    class ENGINE_API input
    {
    protected:
        input() = default;
    public:
        input(const input&) = delete;
        input& operator=(const input&) = delete;

        /// \brief Returns if the specified key is being pressed.
        static bool key_pressed(int p_key_code) { return s_instance->key_pressed_impl(p_key_code); }

        /// \brief Returns if the specified mouse button is being pressed.
        static bool mouse_button_pressed(int p_button) { return s_instance->mouse_button_pressed_impl(p_button); }

        /// \brief Returns the mouse position in window coordinates.
        ///
        /// Always an absolute position. It used to return a *delta* when the
        /// cursor was hidden and an absolute position when it was visible,
        /// which meant callers could not know what they were being given -
        /// the camera treated it as a delta either way and spun wildly
        /// whenever the cursor was shown. Use mouse_delta() for movement.
        static std::pair<float, float> mouse_position() { return s_instance->mouse_position_impl(); }

        /// \brief Returns how far the mouse moved since this was last called.
        ///
        /// Always a delta, whether or not the cursor is visible, with y
        /// already flipped so that up is positive. Returns {0, 0} on the
        /// first call rather than the full cursor position, which would
        /// otherwise snap the camera on the first frame.
        ///
        /// Call it once per frame: it consumes the movement it reports.
        static std::pair<float, float> mouse_delta() { return s_instance->mouse_delta_impl(); }

        /// \brief True while the cursor is captured for looking around.
        ///
        /// Exposed here rather than read off the window, so that callers such
        /// as the camera depend only on input and not on application.
        static bool cursor_captured() { return s_instance->cursor_captured_impl(); }

        /// \brief Returns the x coordinate of the mouse position.
        static float mouse_x() { return s_instance->mouse_x_impl(); }
        /// \brief Returns the y coordinate of the mouse position.
        static float mouse_y() { return s_instance->mouse_y_impl(); }

    protected:
        virtual bool key_pressed_impl(int key_code) const = 0;
        virtual bool mouse_button_pressed_impl(int key_code) const = 0;
        virtual std::pair<float, float> mouse_position_impl() const = 0;
        virtual std::pair<float, float> mouse_delta_impl() const = 0;
        virtual bool cursor_captured_impl() const = 0;
        virtual float mouse_x_impl() const = 0;
        virtual float mouse_y_impl() const = 0;

    private:
        static input* s_instance;
    };
}