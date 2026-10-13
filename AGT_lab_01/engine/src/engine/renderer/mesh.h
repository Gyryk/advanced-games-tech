#pragma once
#include "texture.h"
#include "vertex_array.h"

namespace engine
{
    class mesh
    {
    public:
        struct vertex
        {
            union
            {
                float data[11]{0};
                struct
                {
                    glm::vec3 position;
                    glm::vec3 normal;
                    glm::vec2 tex_coords;
                    /// \brief For normal mapping. Zero for procedural shapes,
                    /// which do not use normal maps; the shader falls back to
                    /// the vertex normal when it is zero.
                    glm::vec3 tangent;
                };
            };
            /// \brief Empty ctor.
            vertex() = default;
            /// \brief Initializer list constructor.
            vertex(std::initializer_list<float> components)
            {
                int i = 0;
                for(const float elem : components)
                {
                    data[i++] = elem;
                }
            }
            /// \brief Specific constructor to allow initialization list with vectors.
            ///
            /// tangent is zeroed explicitly: initialising union members by name
            /// skips the data[] zero-initialiser, so without this it would hold
            /// whatever was on the stack.
            vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex,
                   const glm::vec3& tan = glm::vec3(0.f))
                : position(pos), normal(norm), tex_coords(tex), tangent(tan)
            {
            }
        };

    public:
        virtual ~mesh() = default;

        virtual const ref<vertex_array>& va() const = 0;
        virtual uint32_t indices_count() const = 0;
		virtual std::vector<vertex> vertices() const = 0;
		virtual std::vector<uint32_t> indices() const = 0;

        static ref<mesh> create(
            const std::vector<vertex>& vertices,
            const std::vector<uint32_t>& indices);


    };
}
