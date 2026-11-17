#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <engine.h>
#include "casino_game.h"
#include "casino_assets.h"
#include <array>

// Engine adapter; game rules remain testable without the Windows engine.
class example_layer : public engine::layer
{
public:
    example_layer();
    ~example_layer() override = default;
    void on_update(const engine::timestep& time_step) override;
    void on_render() override;
    void on_event(engine::event& event) override;

private:
    struct prop_model
    {
        engine::ref<engine::game_object> object;
        glm::vec3 origin{0.f};
        float scale = 1.f;
        float yaw = 0.f;
    };

    casino::game m_game;
    engine::perspective_camera m_camera;
    engine::orthographic_camera m_ui_camera{-1.f, 1.f, -1.f, 1.f};
    engine::ref<engine::gl_shader> m_shader;
    engine::ref<engine::shader> m_text_shader;
    engine::ref<engine::text_manager> m_text;
    engine::ref<engine::audio_manager> m_audio;
    engine::ref<engine::skybox> m_skybox;
    engine::ref<engine::mesh> m_box;
    engine::ref<engine::mesh> m_cylinder;
    engine::ref<engine::mesh> m_pyramid;
    engine::ref<engine::mesh> m_floor;
    engine::ref<engine::mesh> m_quad;
    engine::ref<engine::texture_2d> m_white;
    engine::ref<engine::texture_2d> m_paving;
    engine::ref<engine::texture_2d> m_felt;
    engine::ref<engine::texture_2d> m_metal;
    engine::ref<engine::texture_2d> m_chip;
    engine::ref<engine::texture_2d> m_crate;
    engine::ref<engine::texture_2d> m_sign;
    engine::ref<engine::texture_2d> m_exit;
    engine::DirectionalLight m_moon;
    engine::PointLight m_alarm_light;
    std::array<prop_model, 5> m_models;
    std::array<bool, 6> m_sound_loaded{};
    bool m_audio_ready = false;
    bool m_music_loaded = false;
    bool m_muted = false;
    bool m_jump_requested = false;
    float m_accumulator = 0.f;
    float m_visual_time = 0.f;
    float m_camera_yaw = 0.f;
    glm::vec3 m_camera_position{0.f, 12.f, 21.f};
    unsigned int m_width = 0;
    unsigned int m_height = 0;

    void load_assets();
    prop_model load_model(const casino::assets::model_slot& slot);
    void reset_round();
    void update_camera(float dt);
    void play_events();
    void draw_world();
    void draw_hud();
    void draw_mesh(
        const engine::ref<engine::mesh>& mesh,
        glm::vec3 position,
        glm::vec3 scale,
        glm::vec3 colour,
        const engine::ref<engine::texture_2d>& texture,
        float yaw = 0.f,
        bool lit = true);
    void draw_model(
        int slot,
        glm::vec3 position,
        float yaw = 0.f,
        float scale = 1.f);
    void draw_actor(
        casino::vec2 position,
        float height,
        float heading,
        bool guard,
        bool attacking = false);
    void panel(float x, float y, float w, float h, glm::vec4 colour);
    void text(
        const std::string& value,
        float x,
        float y,
        float scale,
        glm::vec4 colour = glm::vec4(1.f));
};
