#include "pch.h"
#include "gl_mesh.h"

engine::gl_mesh::gl_mesh(
    const std::vector<vertex>& vertices,
    const std::vector<uint32_t>& indices)
{
    m_vertices = vertices;
    m_indices = indices;

    // sending vertex data to gpu
    ref<vertex_buffer> vb(vertex_buffer::create((float*) vertices.data(), (uint32_t)vertices.size() * sizeof(vertex)));

    // sending index data to gpu
    ref<index_buffer> ib(index_buffer::create((uint32_t*)indices.data(), (uint32_t)indices.size()));

    // Must match mesh::vertex exactly, in order. The shader binds a_tangent
    // at location 5 (3 and 4 are the skinned-mesh bone attributes).
    const buffer_layout layout
    {
        {e_shader_data_type::float3, "a_position"},
        {e_shader_data_type::float3, "a_normal"},
        {e_shader_data_type::float2, "a_tex_coord"},
        {e_shader_data_type::float3, "a_tangent"},
    };
    vb->layout(layout);

    m_va = engine::vertex_array::create();
    m_va->add_buffer(vb);
    m_va->add_buffer(ib);
}
