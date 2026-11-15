#pragma once

// Game rules deliberately have no engine dependency: the same simulation can
// be exercised without a window, OpenGL context or Windows libraries.
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace casino
{
    struct vec2
    {
        float x = 0.f, z = 0.f;
        vec2 operator+(vec2 b) const { return {x + b.x, z + b.z}; }
        vec2 operator-(vec2 b) const { return {x - b.x, z - b.z}; }
        vec2 operator*(float k) const { return {x * k, z * k}; }
    };
    inline float dot(vec2 a, vec2 b) { return a.x * b.x + a.z * b.z; }
    inline float length(vec2 a) { return std::sqrt(dot(a, a)); }
    inline vec2 unit(vec2 a) { const float n = length(a); return n > .0001f ? a * (1.f / n) : vec2{}; }

    enum class round_state { intro, playing, paused, escaped, caught, timed_out };
    enum class guard_state { patrol, chase, attack, returning };
    enum class pickup_kind { chip, jackpot, security_pass, shield };
    enum class sound_event { collect, alarm, powerup, damage, win, lose };

    struct obstacle { vec2 centre; vec2 half_size; float height; };
    struct pickup { vec2 position; pickup_kind kind; bool active = true; };
    struct guard
    {
        vec2 position;
        std::array<vec2, 4> route;
        int waypoint = 0;
        guard_state state = guard_state::patrol;
        float state_time = 0.f;
        float heading = 0.f;
    };
    struct controls { vec2 movement; bool jump = false; };

    class game
    {
    public:
        static constexpr float arena_x = 12.f, arena_z = 10.f;
        static constexpr float player_radius = .38f;
        static constexpr int target_score = 600;
        static constexpr float round_seconds = 120.f;
        inline static const vec2 exit_position{0.f, 9.f};

        round_state state = round_state::intro;
        vec2 player_position{0.f, 8.f};
        vec2 player_velocity{};
        float player_height = 0.f, vertical_velocity = 0.f, heading = 0.f;
        int score = 0, health = 3;
        float remaining = round_seconds, elapsed = 0.f;
        float shield_time = 0.f, invulnerable_time = 0.f, hit_flash = 0.f;
        bool alarm = false;
        std::vector<pickup> pickups;
        std::vector<guard> guards;
        std::vector<obstacle> obstacles;
        std::vector<sound_event> sounds;

        game() { reset(); state = round_state::intro; }

        void reset()
        {
            state = round_state::playing;
            player_position = {0.f, 8.f}; player_velocity = {};
            player_height = vertical_velocity = heading = 0.f;
            score = 0; health = 3; remaining = round_seconds; elapsed = 0.f;
            shield_time = invulnerable_time = hit_flash = 0.f; alarm = false;
            sounds.clear(); pickups.clear(); guards.clear();
            // Tables have solid footprints; the two low crates can be jumped.
            obstacles = {{{-5.f, -3.f}, {1.8f, 1.f}, 1.25f},
                         {{5.f, -3.f}, {1.8f, 1.f}, 1.25f},
                         {{-5.f, 3.f}, {1.8f, 1.f}, 1.25f},
                         {{5.f, 3.f}, {1.8f, 1.f}, 1.25f},
                         {{-2.f, 0.f}, {.55f, .55f}, .5f},
                         {{2.f, 0.f}, {.55f, .55f}, .5f}};
            // 12 ordinary chips alone suffice: the jackpot is a voluntary risk.
            for (float x : {-9.f, -3.f, 3.f, 9.f})
                for (float z : {-7.f, 0.f, 7.f})
                    pickups.push_back({{x, z}, pickup_kind::chip});
            pickups.push_back({{0.f, -7.f}, pickup_kind::jackpot});
            pickups.push_back({{-10.f, 4.5f}, pickup_kind::security_pass});
            pickups.push_back({{10.f, -4.5f}, pickup_kind::security_pass});
            pickups.push_back({{-10.f, -4.5f}, pickup_kind::shield});
            pickups.push_back({{10.f, 4.5f}, pickup_kind::shield});
            for (float x : {-8.f, 8.f})
                for (float z : {-5.5f, 5.5f})
                {
                    guard g;
                    g.route = {{{x - 1.f, z - 1.f}, {x + 1.f, z - 1.f},
                                {x + 1.f, z + 1.f}, {x - 1.f, z + 1.f}}};
                    g.position = g.route[0]; g.waypoint = 1;
                    guards.push_back(g);
                }
        }

        void toggle_pause()
        {
            if (state == round_state::playing) state = round_state::paused;
            else if (state == round_state::paused) state = round_state::playing;
        }

        // Circle/AABB collision on the ground plane, shared by player and AI.
        bool blocked(vec2 p, float radius, float feet = 0.f) const
        {
            if (std::abs(p.x) > arena_x - radius || std::abs(p.z) > arena_z - radius)
                return true;
            for (const auto& box : obstacles)
            {
                if (feet >= box.height) continue;
                const vec2 nearest{std::clamp(p.x, box.centre.x - box.half_size.x, box.centre.x + box.half_size.x),
                                   std::clamp(p.z, box.centre.z - box.half_size.z, box.centre.z + box.half_size.z)};
                if (length(p - nearest) < radius) return true;
            }
            return false;
        }

        void step(float dt, const controls& input)
        {
            if (state != round_state::playing || dt <= 0.f) return;
            // The caller uses 120 Hz steps. Cap misuse so actors cannot tunnel.
            dt = (std::min)(dt, 1.f / 30.f);
            elapsed += dt;
            remaining = (std::max)(0.f, remaining - dt);
            shield_time = (std::max)(0.f, shield_time - dt);
            invulnerable_time = (std::max)(0.f, invulnerable_time - dt);
            hit_flash = (std::max)(0.f, hit_flash - dt);
            if (remaining <= 0.f) { finish(round_state::timed_out); return; }

            update_player(dt, input);
            collect_pickups();
            // Reaching the exit wins before another guard can tag the player.
            if (score >= target_score && length(player_position - exit_position) < 1.25f)
            { finish(round_state::escaped); return; }
            for (auto& g : guards)
            {
                update_guard(g, dt);
                if (state != round_state::playing) break;
            }
        }

    private:
        void finish(round_state result)
        {
            state = result;
            sounds.push_back(result == round_state::escaped ? sound_event::win : sound_event::lose);
        }

        void update_player(float dt, const controls& input)
        {
            vec2 direction = input.movement;
            if (length(direction) > 1.f) direction = unit(direction);
            // Acceleration plus exponential damping gives inertia and friction
            // without frame-rate-dependent movement. Jump uses gravity.
            player_velocity = (player_velocity + direction * (42.f * dt)) * std::exp(-8.f * dt);
            if (length(direction) > .01f) heading = std::atan2(direction.x, direction.z);
            if (input.jump && player_height <= .001f) vertical_velocity = 6.f;
            vertical_velocity -= 18.f * dt;
            player_height = (std::max)(0.f, player_height + vertical_velocity * dt);
            if (player_height == 0.f) vertical_velocity = 0.f;

            // Resolve axes separately so walking into a table slides along it.
            vec2 next = player_position;
            next.x += player_velocity.x * dt;
            if (!blocked(next, player_radius, player_height)) player_position = next;
            else player_velocity.x = 0.f;
            next = player_position; next.z += player_velocity.z * dt;
            if (!blocked(next, player_radius, player_height)) player_position = next;
            else player_velocity.z = 0.f;

            // A jump can end over a low crate: resolve penetration on landing.
            for (const auto& box : obstacles)
            {
                if (player_height >= box.height) continue;
                const float dx = box.half_size.x + player_radius - std::abs(player_position.x - box.centre.x);
                const float dz = box.half_size.z + player_radius - std::abs(player_position.z - box.centre.z);
                if (dx > 0.f && dz > 0.f && blocked(player_position, player_radius, player_height))
                {
                    if (dx < dz) { player_position.x += (player_position.x >= box.centre.x ? dx : -dx) + (player_position.x >= box.centre.x ? .001f : -.001f); player_velocity.x = 0.f; }
                    else { player_position.z += (player_position.z >= box.centre.z ? dz : -dz) + (player_position.z >= box.centre.z ? .001f : -.001f); player_velocity.z = 0.f; }
                }
            }
        }

        void collect_pickups()
        {
            for (auto& item : pickups)
            {
                if (!item.active || length(player_position - item.position) > .85f || player_height > 1.2f) continue;
                item.active = false;
                switch (item.kind)
                {
                case pickup_kind::chip: score += 50; sounds.push_back(sound_event::collect); break;
                case pickup_kind::jackpot: score += 200; alarm = true; sounds.push_back(sound_event::alarm); break;
                case pickup_kind::security_pass:
                    alarm = false; invulnerable_time = (std::max)(invulnerable_time, 2.f);
                    for (auto& g : guards) { g.state = guard_state::returning; g.state_time = 0.f; }
                    sounds.push_back(sound_event::powerup); break;
                case pickup_kind::shield: shield_time = 8.f; sounds.push_back(sound_event::powerup); break;
                }
            }
        }

        void steer(guard& g, vec2 target, float speed, float dt)
        {
            const vec2 desired = unit(target - g.position);
            if (length(desired) < .01f) return;
            // Local obstacle avoidance is enough for this open terrace. Probe
            // both sides of a table rather than adding a navigation mesh.
            for (float angle : {0.f, .6f, -.6f, 1.2f, -1.2f, 1.57f, -1.57f, 2.4f, -2.4f})
            {
                const vec2 direction{desired.x * std::cos(angle) - desired.z * std::sin(angle),
                                     desired.x * std::sin(angle) + desired.z * std::cos(angle)};
                if (blocked(g.position + direction * .7f, .4f)) continue;
                const vec2 next = g.position + direction * ((std::min)(speed * dt, length(target - g.position)));
                if (blocked(next, .4f)) continue;
                g.position = next; g.heading = std::atan2(direction.x, direction.z); return;
            }
        }

        void update_guard(guard& g, float dt)
        {
            g.state_time += dt;
            const float distance = length(player_position - g.position);
            const float detection = alarm ? 9.f : 4.4f;
            const float speed = alarm ? 3.8f : 2.5f;
            switch (g.state)
            {
            case guard_state::patrol:
                if (distance < detection) { g.state = guard_state::chase; g.state_time = 0.f; }
                else
                {
                    if (length(g.position - g.route[g.waypoint]) < .25f) g.waypoint = (g.waypoint + 1) % 4;
                    steer(g, g.route[g.waypoint], 1.3f, dt);
                }
                break;
            case guard_state::chase:
                if (distance > detection + 3.f) { g.state = guard_state::returning; g.state_time = 0.f; }
                else if (distance < 1.f) { g.state = guard_state::attack; g.state_time = 0.f; }
                else steer(g, player_position, speed, dt);
                break;
            case guard_state::attack:
                // Telegraph the tag, giving the player time to step or jump away.
                if (g.state_time >= .4f)
                {
                    if (distance < 1.25f && player_height < .55f && shield_time <= 0.f && invulnerable_time <= 0.f)
                    {
                        --health; invulnerable_time = 1.8f; hit_flash = .45f;
                        player_velocity = unit(player_position - g.position) * 9.f;
                        sounds.push_back(sound_event::damage);
                        if (health <= 0) finish(round_state::caught);
                    }
                    g.state = guard_state::returning; g.state_time = 0.f;
                }
                break;
            case guard_state::returning:
                steer(g, g.route[g.waypoint], 1.8f, dt);
                // Recovery prevents repeated contact damage and makes a stolen
                // security pass useful even if a guard was already pursuing.
                if (g.state_time > 2.f && length(g.position - g.route[g.waypoint]) < .3f)
                { g.state = guard_state::patrol; g.state_time = 0.f; }
                else if (g.state_time > 3.f && distance < detection)
                { g.state = guard_state::chase; g.state_time = 0.f; }
                break;
            }
        }
    };
}
