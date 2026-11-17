#pragma once
#include <array>

namespace casino::assets
{
    // Paths are relative to game/, the Visual Studio working directory.
    // Keep downloaded models with their companion MTL and texture files.
    struct model_slot
    {
        const char* path;
        float height;
        float yaw_degrees;
    };
    // These settings are defined in a header included by several .cpp files.
    // 'inline' allows the shared definitions; 'constexpr' makes their values
    // fixed at compile time. Edit them here, then rebuild to change an asset.
    inline constexpr model_slot chair{"assets/casino/models/chair.obj", 1.3f, 0.f};
    inline constexpr model_slot lamp{"assets/casino/models/lamp.obj", 3.f, 0.f};
    inline constexpr model_slot planter{"assets/casino/models/planter.obj", 1.4f, 0.f};
    // Empty paths select the block characters. Static replacement models should
    // face +Z, be Y-up and have transforms baked. Adjust yaw if necessary.
    inline constexpr model_slot thief{"", 1.6f, 0.f};
    inline constexpr model_slot guard{"", 1.7f, 0.f};
    inline constexpr const char* paving = "assets/casino/textures/paving.png";
    inline constexpr const char* felt = "assets/casino/textures/felt.png";
    inline constexpr const char* metal = "assets/casino/textures/metal.png";
    inline constexpr const char* chip = "assets/casino/textures/chip.png";
    inline constexpr const char* crate = "assets/casino/textures/crate.png";
    inline constexpr const char* sign = "assets/casino/textures/casino.png";
    inline constexpr const char* exit_sign = "assets/casino/textures/exit.png";
    // Engine skybox order: front, right, back, left, top, bottom.
    inline constexpr std::array<const char*, 6> sky = {{
        "assets/casino/textures/sky_front.png", "assets/casino/textures/sky_right.png",
        "assets/casino/textures/sky_back.png", "assets/casino/textures/sky_left.png",
        "assets/casino/textures/sky_top.png", "assets/casino/textures/sky_bottom.png"}};
    inline constexpr std::array<const char*, 6> event_sounds = {{
        "assets/casino/audio/collect.wav", "assets/casino/audio/alarm.wav",
        "assets/casino/audio/powerup.wav", "assets/casino/audio/damage.wav",
        "assets/casino/audio/win.wav", "assets/casino/audio/lose.wav"}};
    inline constexpr const char* music = "assets/casino/audio/ambience.wav";
}
