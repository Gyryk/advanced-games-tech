#pragma once

#include "engine/core/input.h"

namespace engine
{
    class win_input : public input
    {
    public:
        bool key_pressed_impl(int32_t key_code) const override;
        bool mouse_button_pressed_impl(int32_t button) const override;

    protected:
        std::pair<float, float> mouse_position_impl() const override;
        std::pair<float, float> mouse_delta_impl() const override;
        bool cursor_captured_impl() const override;
        float mouse_x_impl() const override;
        float mouse_y_impl() const override;

    private:
        // Where the cursor was when mouse_delta_impl() last ran. Updated on
        // every call regardless of cursor visibility - the previous version
        // only updated it while the cursor was hidden, so showing the cursor
        // and hiding it again produced one enormous delta from a stale
        // reference point.
        inline static std::pair<float, float> s_last_position;
        inline static bool s_has_last_position = false;
    };
}
