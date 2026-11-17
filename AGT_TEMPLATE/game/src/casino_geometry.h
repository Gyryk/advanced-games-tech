#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include <cstdint>
#include <vector>

namespace casino
{
    struct vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };
    struct geometry
    {
        std::vector<vertex> vertices;
        std::vector<uint32_t> indices;
    };

    // 'inline' permits these header-defined functions to be included by multiple
    // .cpp files. It does not force the compiler to optimise away function calls.
    inline void quad(geometry& g, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, float repeat = 1.f)
    {
        const auto base = static_cast<uint32_t>(g.vertices.size());
        const auto normal = glm::normalize(glm::cross(b - a, c - a));
        g.vertices.push_back({a, normal, {0, 0}});
        g.vertices.push_back({b, normal, {repeat, 0}});
        g.vertices.push_back({c, normal, {repeat, repeat}});
        g.vertices.push_back({d, normal, {0, repeat}});
        g.indices.insert(g.indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
    }

    inline geometry box_geometry()
    {
        geometry g;
        quad(g, {-.5f,-.5f,.5f}, {.5f,-.5f,.5f}, {.5f,.5f,.5f}, {-.5f,.5f,.5f});
        quad(g, {.5f,-.5f,-.5f}, {-.5f,-.5f,-.5f}, {-.5f,.5f,-.5f}, {.5f,.5f,-.5f});
        quad(g, {.5f,-.5f,.5f}, {.5f,-.5f,-.5f}, {.5f,.5f,-.5f}, {.5f,.5f,.5f});
        quad(g, {-.5f,-.5f,-.5f}, {-.5f,-.5f,.5f}, {-.5f,.5f,.5f}, {-.5f,.5f,-.5f});
        quad(g, {-.5f,.5f,.5f}, {.5f,.5f,.5f}, {.5f,.5f,-.5f}, {-.5f,.5f,-.5f});
        quad(g, {-.5f,-.5f,-.5f}, {.5f,-.5f,-.5f}, {.5f,-.5f,.5f}, {-.5f,-.5f,.5f});
        return g;
    }

    inline geometry cylinder_geometry()
    {
        geometry g;
        const int sides = 24;
        const float tau = 6.28318530718f;
        for (int i = 0; i <= sides; ++i)
        {
            const float u = static_cast<float>(i) / sides, a = tau * u;
            const glm::vec3 n{std::cos(a), 0.f, std::sin(a)};
            g.vertices.push_back({{n.x * .5f, -.5f, n.z * .5f}, n, {u, 0}});
            g.vertices.push_back({{n.x * .5f, .5f, n.z * .5f}, n, {u, 1}});
            if (i < sides)
            {
                const auto k = static_cast<uint32_t>(2 * i);
                g.indices.insert(g.indices.end(), {k, k + 1, k + 2, k + 1, k + 3, k + 2});
            }
        }
        // Caps need separate vertices: normals and UVs differ from the rim.
        for (float sign : {-1.f, 1.f})
        {
            const auto base = static_cast<uint32_t>(g.vertices.size());
            g.vertices.push_back({{0.f, sign * .5f, 0.f}, {0.f, sign, 0.f}, {.5f,.5f}});
            for (int i = 0; i <= sides; ++i)
            {
                const float a = tau * static_cast<float>(i) / sides;
                const float x = .5f * std::cos(a), z = .5f * std::sin(a);
                g.vertices.push_back({{x, sign * .5f, z}, {0.f, sign, 0.f}, {x + .5f, z + .5f}});
                if (i < sides)
                {
                    const auto k = base + 1 + static_cast<uint32_t>(i);
                    if (sign > 0.f) g.indices.insert(g.indices.end(), {base, k + 1, k});
                    else g.indices.insert(g.indices.end(), {base, k, k + 1});
                }
            }
        }
        return g;
    }

    inline geometry pyramid_geometry()
    {
        geometry g;
        const glm::vec3 ring[] = {{-.5f,-.5f,.5f}, {.5f,-.5f,.5f}, {.5f,-.5f,-.5f}, {-.5f,-.5f,-.5f}};
        const glm::vec3 tip{0.f,.5f,0.f};
        for (int i = 0; i < 4; ++i)
        {
            const auto a = ring[i], b = ring[(i + 1) % 4];
            const auto n = glm::normalize(glm::cross(b - a, tip - a));
            const auto k = static_cast<uint32_t>(g.vertices.size());
            g.vertices.push_back({a, n, {0, 0}});
            g.vertices.push_back({b, n, {1, 0}});
            g.vertices.push_back({tip, n, {.5f, 1}});
            g.indices.insert(g.indices.end(), {k,k+1,k+2});
        }
        quad(g, ring[3], ring[2], ring[1], ring[0]);
        return g;
    }

    inline geometry floor_geometry()
    {
        geometry g;
        // Repeat the paving rather than stretching one image over the terrace.
        quad(g, {-12.f,0.f,10.f}, {12.f,0.f,10.f}, {12.f,0.f,-10.f}, {-12.f,0.f,-10.f}, 12.f);
        // 24/12 == 20/10: square texture tiles stay square on this rectangle.
        g.vertices[2].uv.y = g.vertices[3].uv.y = 10.f;
        return g;
    }
}
