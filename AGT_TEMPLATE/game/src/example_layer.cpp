#include "example_layer.h"
#include "casino_geometry.h"
#include "engine/events/key_event.h"
#include <gl/GL.h> // Windows SDK: only legacy depth/culling state calls are needed.
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>
#include <iostream>

namespace
{
    constexpr float fixed_step = 1.f / 120.f;
    const glm::vec3 gold{1.f, .72f, .22f};
    const glm::vec3 cyan{.25f, .9f, 1.f};
    const std::array<const char*, 6> sound_names{{"collect", "alarm", "powerup", "damage", "win", "lose"}};

    engine::ref<engine::mesh> upload(const casino::geometry& source)
    {
        std::vector<engine::mesh::vertex> vertices;
        vertices.reserve(source.vertices.size());
        for (const auto& v : source.vertices) vertices.emplace_back(v.position, v.normal, v.uv);
        return engine::mesh::create(vertices, source.indices);
    }

    engine::ref<engine::texture_2d> texture_or_white(const char* path, bool clamp = false)
    {
        if (std::filesystem::is_regular_file(path))
        {
            auto texture = engine::texture_2d::create(path, clamp);
            if (texture->loaded()) return texture;
        }
        std::cerr << "[casino] Missing/invalid texture: " << path << "; using white.\n";
        return engine::texture_2d::create(255, 255, 255);
    }
}

example_layer::example_layer()
    : m_camera(static_cast<float>(engine::application::window().width()),
               static_cast<float>(engine::application::window().height()), 55.f, .1f, 180.f)
{
    engine::application::window().show_mouse_cursor();
    engine::application::window().vsync(true);
    m_shader = std::dynamic_pointer_cast<engine::gl_shader>(engine::renderer::shaders_library()->get("mesh"));
    m_text_shader = engine::renderer::shaders_library()->get("text_2D");
    load_assets();
    m_text = engine::text_manager::create();
    m_moon.Color = { .8f, .88f, 1.f };
    m_moon.AmbientIntensity = .45f;
    m_moon.DiffuseIntensity = .65f;
    m_moon.Direction = glm::normalize(glm::vec3(-.3f, -1.f, -.4f));
    m_alarm_light.Position = {0.f, 4.f, -8.f};
    m_alarm_light.AmbientIntensity = .08f;
    m_alarm_light.DiffuseIntensity = 1.8f;
    m_alarm_light.Attenuation.Constant = 1.f;
    m_alarm_light.Attenuation.Linear = .07f;
    m_alarm_light.Attenuation.Exp = .025f;
    update_camera(0.f);
}

void example_layer::load_assets()
{
    using namespace casino::assets;
    m_box = upload(casino::box_geometry());
    m_cylinder = upload(casino::cylinder_geometry());
    m_pyramid = upload(casino::pyramid_geometry());
    m_floor = upload(casino::floor_geometry());
    casino::geometry ui_quad;
    casino::quad(ui_quad, {-.5f,-.5f,0.f}, {.5f,-.5f,0.f}, {.5f,.5f,0.f}, {-.5f,.5f,0.f});
    m_quad = upload(ui_quad);
    m_white = engine::texture_2d::create(255, 255, 255);
    m_paving = texture_or_white(paving); m_felt = texture_or_white(felt);
    m_metal = texture_or_white(metal); m_chip = texture_or_white(chip);
    m_crate = texture_or_white(crate); m_sign = texture_or_white(sign, true);
    m_exit = texture_or_white(exit_sign, true);
    std::vector<engine::ref<engine::texture_2d>> sky_textures;
    for (const char* path : sky) sky_textures.push_back(texture_or_white(path, true));
    m_skybox = engine::skybox::create(60.f, sky_textures);
    m_models = {{load_model(chair), load_model(lamp), load_model(planter), load_model(thief), load_model(guard)}};
    m_audio = engine::audio_manager::instance();
    m_audio_ready = m_audio->init();
    if (!m_audio_ready) { std::cerr << "[casino] Audio unavailable; continuing silently.\n"; return; }
    for (size_t i = 0; i < sound_names.size(); ++i)
        if (std::filesystem::is_regular_file(event_sounds[i]))
            m_sound_loaded[i] = m_audio->load_sound(event_sounds[i], engine::sound_type::event, sound_names[i]);
    if (std::filesystem::is_regular_file(music))
        m_music_loaded = m_audio->load_sound(music, engine::sound_type::track, "ambience");
    if (m_music_loaded) { m_audio->play("ambience"); m_audio->volume("ambience", .22f); }
}

example_layer::prop_model example_layer::load_model(const casino::assets::model_slot& slot)
{
    prop_model result;
    if (!slot.path[0]) return result;
    if (!std::filesystem::is_regular_file(slot.path))
    { std::cerr << "[casino] Model not found: " << slot.path << "; using a primitive.\n"; return result; }
    const auto model = engine::model::create(slot.path);
    if (model->meshes().empty() || model->size().y < .001f) return result;
    engine::game_object_properties props;
    props.meshes = model->meshes();
    props.textures = model->textures();
    props.mesh_materials = model->mesh_materials();
    // Supply a safe diffuse fallback for models that only contain colours.
    // Proper replacement textures are still needed for the final artwork.
    for (auto& mat : props.mesh_materials) if (!mat.diffuse) mat.diffuse = m_white;
    result.object = engine::game_object::create(props);
    result.scale = slot.height / model->size().y;
    result.origin = model->offset() - glm::vec3(0.f, model->size().y * .5f, 0.f);
    result.yaw = glm::radians(slot.yaw_degrees);
    return result;
}

void example_layer::reset_round()
{
    m_game.reset(); m_accumulator = 0.f; m_jump_requested = false; m_camera_yaw = 0.f;
    m_camera_position = {0.f, 12.f, 21.f};
    update_camera(0.f);
}

void example_layer::on_update(const engine::timestep& time_step)
{
    const float dt = std::clamp(static_cast<float>(time_step), 0.f, .1f);
    m_visual_time += dt;
    using key = engine::key_codes;
    if (m_game.state == casino::round_state::playing)
    {
        const auto pressed = [](int k) { return engine::input::key_pressed(k) ? 1.f : 0.f; };
        m_camera_yaw += (pressed(key::KEY_E) - pressed(key::KEY_Q)) * 1.4f * dt;
        const float horizontal = pressed(key::KEY_D) - pressed(key::KEY_A);
        const float vertical = pressed(key::KEY_S) - pressed(key::KEY_W);
        // Movement remains relative to the orbit camera, so W always moves away.
        casino::controls input;
        input.movement = {horizontal * std::cos(m_camera_yaw) + vertical * std::sin(m_camera_yaw),
                          -horizontal * std::sin(m_camera_yaw) + vertical * std::cos(m_camera_yaw)};
        m_accumulator += dt;
        while (m_accumulator >= fixed_step)
        {
            input.jump = m_jump_requested;
            m_game.step(fixed_step, input);
            m_jump_requested = false;
            m_accumulator -= fixed_step;
        }
    }
    else { m_accumulator = 0.f; m_jump_requested = false; }
    update_camera(dt);
    play_events();
    if (m_audio_ready) m_audio->on_update();
}

void example_layer::update_camera(float dt)
{
    const auto width = engine::application::window().width();
    const auto height = engine::application::window().height();
    if (width && height && (m_width != width || m_height != height))
    {
        m_width = width; m_height = height;
        m_camera = engine::perspective_camera(static_cast<float>(width), static_cast<float>(height), 55.f, .1f, 180.f);
    }
    const glm::vec3 target{m_game.player_position.x, .65f, m_game.player_position.z};
    const glm::vec3 desired = target + glm::vec3(13.f * std::sin(m_camera_yaw), 12.f, 13.f * std::cos(m_camera_yaw));
    m_camera_position = dt > 0.f ? glm::mix(m_camera_position, desired, 1.f - std::exp(-7.f * dt)) : desired;
    // Shake affects the view only; it cannot alter gameplay or accumulate drift.
    const float shake = m_game.state == casino::round_state::playing ? m_game.hit_flash * .35f : 0.f;
    const glm::vec3 offset{std::sin(m_visual_time * 83.f) * shake, std::cos(m_visual_time * 71.f) * shake, 0.f};
    m_camera.set_view_matrix(m_camera_position + offset, target);
}

void example_layer::play_events()
{
    if (m_audio_ready && !m_muted)
        for (const auto event : m_game.sounds)
        {
            const auto index = static_cast<size_t>(event);
            if (m_sound_loaded[index]) m_audio->play(sound_names[index]);
        }
    m_game.sounds.clear();
}

void example_layer::draw_mesh(const engine::ref<engine::mesh>& mesh, glm::vec3 position, glm::vec3 scale,
                              glm::vec3 colour, const engine::ref<engine::texture_2d>& texture, float yaw, bool lit)
{
    auto transform = glm::translate(glm::mat4(1.f), position);
    transform = glm::rotate(transform, yaw, glm::vec3(0.f, 1.f, 0.f));
    transform = glm::scale(transform, scale);
    m_shader->set_uniform("lighting_on", lit);
    m_shader->set_uniform("has_texture", true);
    m_shader->set_uniform("colouring_on", true);
    m_shader->set_uniform("in_colour", colour);
    texture->bind(0);
    engine::renderer::submit(m_shader, mesh, transform);
}

void example_layer::draw_model(int slot, glm::vec3 position, float yaw, float scale)
{
    const auto& prop = m_models[slot];
    if (!prop.object)
    {
        draw_mesh(m_box, position + glm::vec3(0.f,.5f,0.f), glm::vec3(.8f,1.f,.8f) * scale, gold, m_crate, yaw);
        return;
    }
    auto transform = glm::translate(glm::mat4(1.f), position);
    transform = glm::rotate(transform, yaw + prop.yaw, glm::vec3(0.f,1.f,0.f));
    transform = glm::scale(transform, glm::vec3(prop.scale * scale));
    transform = glm::translate(transform, -prop.origin);
    m_shader->set_uniform("colouring_on", false);
    m_shader->set_uniform("lighting_on", true);
    m_shader->set_uniform("has_normal_map", false);
    engine::renderer::submit(m_shader, transform, prop.object);
}

void example_layer::draw_actor(casino::vec2 p, float height, float heading, bool guard, bool attacking)
{
    const int slot = guard ? 4 : 3;
    const glm::vec3 base{p.x, height, p.z};
    if (m_models[slot].object) { draw_model(slot, base, heading); return; }
    const glm::vec3 colour = guard ? (attacking ? glm::vec3(1.f,.18f,.1f) : glm::vec3(.25f,.42f,.8f)) : glm::vec3(.16f,.18f,.22f);
    draw_mesh(m_box, base + glm::vec3(0,.68f,0), {.62f,.92f,.46f}, colour, m_metal, heading);
    draw_mesh(m_cylinder, base + glm::vec3(0,1.38f,0), {.45f,.42f,.45f}, guard ? gold : cyan, m_metal, heading);
    const glm::vec3 forward{std::sin(heading),0.f,std::cos(heading)};
    draw_mesh(m_box, base + glm::vec3(0,1.38f,0) + forward * .23f, {.32f,.1f,.04f}, {.04f,.04f,.06f}, m_white, heading);
    for (float side : {-.17f,.17f})
    {
        const glm::vec3 right{std::cos(heading),0.f,-std::sin(heading)};
        draw_mesh(m_box, base + glm::vec3(0,.16f,0) + right * side, {.22f,.32f,.3f}, colour, m_metal, heading);
    }
}

void example_layer::draw_world()
{
    m_shader->set_uniform("transparency", 1.f);
    m_shader->set_uniform("num_bones", 0);
    m_shader->set_uniform("fog_on", false);
    m_shader->set_uniform("has_specular_map", false);
    m_shader->set_uniform("has_normal_map", false);
    m_shader->set_uniform("gColorMap", 0);
    m_shader->set_uniform("gEyeWorldPos", m_camera.position());
    m_shader->set_uniform("gMatSpecularIntensity", .12f);
    m_shader->set_uniform("gSpecularPower", 24.f);
    m_shader->set_uniform("gNumPointLights", 1);
    m_shader->set_uniform("gNumSpotLights", 0);
    m_moon.submit(m_shader);
    m_alarm_light.Color = m_game.alarm ? glm::vec3(1.f,.08f,.04f) : gold;
    m_alarm_light.DiffuseIntensity = m_game.alarm ? 1.5f + .8f * std::sin(m_visual_time * 8.f) : 1.8f;
    m_alarm_light.submit(m_shader, 0);
    m_shader->set_uniform("colouring_on", false);
    glDepthMask(GL_FALSE);
    engine::renderer::submit(m_shader, m_skybox, glm::translate(glm::mat4(1.f), m_camera.position()));
    glDepthMask(GL_TRUE);

    draw_mesh(m_box, {0,-.3f,0}, {29,.5f,25}, {.14f,.15f,.2f}, m_metal);
    draw_mesh(m_floor, {0,0,0}, {1,1,1}, {1,1,1}, m_paving);
    // Boundary walls match the simulation limits. Outer props are decorative.
    for (float side : {-1.f,1.f})
    {
        draw_mesh(m_box, {side * 12.2f,.4f,0}, {.4f,.8f,20.4f}, {.35f,.28f,.25f}, m_crate);
        draw_mesh(m_box, {side * 7.f,.4f,10.2f}, {10.f,.8f,.4f}, {.35f,.28f,.25f}, m_crate);
        draw_model(0, {side * 13.2f,0,4.f}, side * 1.57f);
        draw_model(0, {side * 13.2f,0,-4.f}, side * 1.57f, .9f);
        draw_model(1, {side * 13.1f,0,8.f}, 0.f);
        draw_model(1, {side * 13.1f,0,-8.f}, 0.f, .85f);
        draw_model(2, {side * 9.f,0,-11.4f}, side * .3f);
        draw_model(2, {side * 5.f,0,-11.4f}, side * .7f, .8f);
    }
    draw_mesh(m_box, {0,2.f,-10.8f}, {25,4,1.4f}, {.2f,.16f,.2f}, m_metal);
    draw_mesh(m_box, {0,3.1f,-10.05f}, {9,1.3f,.1f}, {1,1,1}, m_sign, 0.f, false);
    draw_mesh(m_box, {0,.025f,7.8f}, {2.4f,.03f,4.2f}, {.45f,.03f,.08f}, m_felt);
    const bool exit_open = m_game.score >= casino::game::target_score;
    draw_mesh(m_box, {0,.05f,9.1f}, {2.4f,.04f,1.5f}, exit_open ? glm::vec3(.1f,1.f,.5f) : gold, m_exit, 0.f, false);
    for (const auto& box : m_game.obstacles)
    {
        const glm::vec3 pos{box.centre.x, box.height * .5f, box.centre.z};
        if (box.height > 1.f)
        {
            draw_mesh(m_box, pos, {box.half_size.x * 2.f,box.height,box.half_size.z * 2.f}, {.18f,.11f,.1f}, m_crate);
            draw_mesh(m_box, {pos.x,box.height + .02f,pos.z}, {box.half_size.x * 2.f,.04f,box.half_size.z * 2.f}, {1,1,1}, m_felt);
            for (float x : {-1.f,0.f,1.f})
                draw_mesh(m_cylinder, {pos.x+x,box.height+.08f,pos.z}, {.23f,.1f,.23f}, gold, m_chip);
        }
        else draw_mesh(m_box, pos, {box.half_size.x*2.f,box.height,box.half_size.z*2.f}, {.8f,.6f,.3f}, m_crate);
    }
    for (const auto& item : m_game.pickups)
    {
        if (!item.active) continue;
        const glm::vec3 position{item.position.x, .45f + .09f * std::sin(m_visual_time * 3.f + item.position.x), item.position.z};
        switch (item.kind)
        {
        case casino::pickup_kind::chip:
            draw_mesh(m_cylinder, position, {.7f,.16f,.7f}, {.95f,.24f,.28f}, m_chip, m_visual_time); break;
        case casino::pickup_kind::jackpot:
            draw_mesh(m_cylinder, position, {1.f,.3f,1.f}, gold, m_chip, -m_visual_time); break;
        case casino::pickup_kind::security_pass:
            draw_mesh(m_box, position, {.7f,.08f,.45f}, cyan, m_chip, m_visual_time); break;
        case casino::pickup_kind::shield:
            draw_mesh(m_pyramid, position + glm::vec3(0,.15f,0), {.65f,.9f,.65f}, {.3f,1.f,.45f}, m_metal, m_visual_time); break;
        }
    }
    draw_actor(m_game.player_position, m_game.player_height, m_game.heading, false);
    if (m_game.shield_time > 0.f || m_game.invulnerable_time > 0.f)
        draw_mesh(m_cylinder, {m_game.player_position.x,.035f,m_game.player_position.z}, {1.25f,.04f,1.25f}, cyan, m_chip, m_visual_time, false);
    for (const auto& guard : m_game.guards)
        draw_actor(guard.position, 0.f, guard.heading, true, guard.state == casino::guard_state::attack);
}

void example_layer::panel(float x, float y, float w, float h, glm::vec4 colour)
{
    // UI coordinates use a 1280 x 720 reference canvas, origin at the top left.
    m_shader->bind();
    // colouring_on also multiplies alpha, so use sqrt to apply it exactly once overall.
    m_shader->set_uniform("transparency", std::sqrt(colour.a));
    draw_mesh(m_quad, {(x+w*.5f)/640.f-1.f,1.f-(y+h*.5f)/360.f,0.f}, {w/640.f,h/360.f,1.f}, glm::vec3(colour), m_white, 0.f, false);
    m_shader->set_uniform("transparency", 1.f);
}

void example_layer::text(const std::string& value, float x, float y, float scale, glm::vec4 colour)
{
    const float factor = (std::min)(static_cast<float>(m_width)/1280.f, static_cast<float>(m_height)/720.f);
    m_text->render_text(m_text_shader, value, x * static_cast<float>(m_width)/1280.f,
                       (720.f-y) * static_cast<float>(m_height)/720.f, scale * factor, colour);
}

void example_layer::draw_hud()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    engine::renderer::begin_scene(m_ui_camera, m_shader);
    auto text_gl = std::dynamic_pointer_cast<engine::gl_shader>(m_text_shader);
    text_gl->bind();
    text_gl->set_uniform("projection", glm::ortho(0.f, static_cast<float>(m_width), 0.f, static_cast<float>(m_height)));
    panel(0,0,1280,100,{.025f,.035f,.06f,.9f});
    text("THE HOUSE COLLECTS",24,35,.55f,glm::vec4(gold,1));
    text("CHIPS  $" + std::to_string(m_game.score) + " / $600",24,75,.48f);
    text("HEALTH  " + std::to_string(m_game.health) + " / 3",410,75,.48f);
    text("TIME  " + std::to_string(static_cast<int>(std::ceil(m_game.remaining))),720,75,.48f);
    text(m_game.alarm ? "ALARM / HARD" : "QUIET / EASY",990,75,.48f,
         m_game.alarm ? glm::vec4(1,.3f,.2f,1) : glm::vec4(.35f,1,.65f,1));
    panel(0,658,1280,62,{.025f,.035f,.06f,.9f});
    text("WASD move   SPACE jump   Q/E orbit   P pause   R restart   M mute   ESC quit",24,696,.38f);
    if (m_game.state == casino::round_state::playing)
    {
        text(m_game.score >= casino::game::target_score ? "TARGET REACHED - return to the green EXIT pad!" : "Collect chips, then return to the EXIT with $600.",24,134,.43f,glm::vec4(gold,1));
        if (m_game.shield_time > 0.f) text("SHIELD  " + std::to_string(static_cast<int>(std::ceil(m_game.shield_time))) + "s",24,169,.43f,glm::vec4(cyan,1));
        if (m_game.hit_flash > 0.f) panel(0,100,1280,558,{.85f,.02f,.01f,m_game.hit_flash*.45f});
    }
    else
    {
        panel(160,177,960,430,{.025f,.03f,.05f,.96f});
        if (m_game.state == casino::round_state::intro)
        {
            text("THE HOUSE COLLECTS",220,245,.85f,glm::vec4(gold,1));
            text("Steal $600 in chips. Reach the exit before security catches you.",220,298,.42f);
            text("RED chips: $50     GOLD chip: $200, but triggers the alarm",220,346,.42f);
            text("BLUE pass: clears alarm     GREEN pyramid: 8-second shield",220,388,.42f);
            text("Guards flash red before a tag. Jump or move away to dodge.",220,430,.42f);
            text("WASD move   SPACE jump   Q/E orbit camera",220,472,.42f);
            text("PRESS ENTER TO START",220,550,.65f,glm::vec4(cyan,1));
        }
        else if (m_game.state == casino::round_state::paused)
        {
            text("PAUSED",220,280,.9f,glm::vec4(gold,1));
            text("P to resume     R to restart     ESC to quit",220,370,.5f);
        }
        else
        {
            const bool won = m_game.state == casino::round_state::escaped;
            text(won ? "YOU GOT AWAY" : "THE HOUSE WINS",220,280,.85f,glm::vec4(gold,1));
            text(won ? "You escaped with the chips." : (m_game.state == casino::round_state::caught ? "Security caught you." : "Time ran out."),220,350,.5f);
            text("Collected: $" + std::to_string(m_game.score),220,405,.5f);
            text("ENTER / R to try again     ESC to quit",220,510,.55f,glm::vec4(cyan,1));
        }
    }
    if (m_muted) text("MUTED",1110,35,.38f);
    engine::renderer::end_scene();
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void example_layer::on_render()
{
    if (!m_width || !m_height) return;
    engine::render_command::clear_color({.025f,.035f,.065f,1.f});
    engine::render_command::clear();
    engine::renderer::begin_scene(m_camera, m_shader);
    draw_world();
    engine::renderer::end_scene();
    draw_hud();
}

void example_layer::on_event(engine::event& event)
{
    if (event.event_type() != engine::event_type_e::key_pressed) return;
    const auto& key = static_cast<engine::key_pressed_event&>(event);
    if (key.repeats_count() != 0) return;
    using keys = engine::key_codes;
    switch (key.key_code())
    {
    case keys::KEY_ENTER:
        if (m_game.state != casino::round_state::playing && m_game.state != casino::round_state::paused) reset_round();
        break;
    case keys::KEY_R: reset_round(); break;
    case keys::KEY_P: m_game.toggle_pause(); break;
    case keys::KEY_SPACE: if (m_game.state == casino::round_state::playing) m_jump_requested = true; break;
    case keys::KEY_M:
        m_muted = !m_muted;
        if (m_music_loaded) m_audio->volume("ambience", m_muted ? 0.f : .22f);
        break;
    default: break;
    }
}
