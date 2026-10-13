#include "pch.h"
#include "renderer.h"
#include "platform/opengl/gl_shader.h"
#include "engine/renderer/skinned_mesh.h"

engine::renderer::scene_data* engine::renderer::s_scene_data = new scene_data;
engine::renderer* engine::renderer::s_instance = new renderer();

void engine::renderer::init()
{
    s_instance->m_shader_library = std::make_unique<shader_library>();
    renderer_api::init();

	renderer::shaders_library()->load("assets/shaders/mesh.glsl");
	renderer::shaders_library()->load("assets/shaders/colour.glsl");
	renderer::shaders_library()->load("assets/shaders/text_2D.glsl");
}

void engine::renderer::resize(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    render_command::resize_viewport(x,y,width, height);
}

void engine::renderer::begin_scene(camera& camera, const ref<shader>& shader)
{
    s_scene_data->view_projection_matrix = camera.view_projection_matrix();
    s_scene_data->shader = shader;
    shader->bind();
    std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("u_view_projection", s_scene_data->view_projection_matrix);
}

void engine::renderer::end_scene()
{
    s_scene_data->shader->unbind(); 
}

void engine::renderer::submit(
    const ref<shader>& shader, 
    const ref<vertex_array>& vertex_array, 
    const glm::mat4& transform /*= glm::mat4(1.f)*/)
{
    std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("u_transform", transform);

    vertex_array->bind();
    render_command::submit(vertex_array);
}

void engine::renderer::submit(
    const ref<shader>& shader,
    const ref<mesh>& mesh,
    const glm::mat4& transform /*= glm::mat4(1.f)*/)
{
    submit(shader, mesh->va(), transform);
}

void engine::renderer::submit(
    const ref<shader>& shader, 
	const ref<game_object>& object)
{
	if (object->textures().size() > 0)
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);

	glm::mat4 transform = glm::mat4(1.0f);
	object->transform(transform);

	bool texture_per_mesh = false;
	if (object->textures().size() == object->meshes().size()) texture_per_mesh = true;
	else object->bind_textures();

    const bool has_meshes = !object->meshes().empty();
	if (has_meshes)
	{
		auto model_meshes = object->meshes();
		int i = 0;
		for (const auto& mesh : model_meshes)
		{
			if(texture_per_mesh) object->textures().at(i)->bind();
			submit(shader, mesh, transform);
			i++;
		}
	}
	else if (object->animated_mesh() != NULL)
	{
		object->animated_mesh()->on_render(transform, shader);
	}

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
}

void engine::renderer::submit(
	const ref<shader>& shader,
	const glm::mat4& transform,
	const ref<game_object>& object )
{
	auto gl = std::dynamic_pointer_cast<engine::gl_shader>(shader);

	if (object->textures().size() > 0)
		gl->set_uniform("has_texture", true);

	// Per-mesh materials, when the object came from a model, are authoritative:
	// each mesh names its own textures via the material index assimp gave us.
	//
	// The fallback below is the old behaviour, kept for objects assembled by
	// hand from procedural shapes, where lesson code sets textures in mesh
	// order deliberately. It pairs texture i with mesh i only when the two
	// counts happen to match, and otherwise binds every texture in turn so the
	// last one wins - which is why a model whose meshes shared a texture, or
	// which had both a diffuse and a specular map, rendered incorrectly.
	const auto& materials = object->mesh_materials();
	const bool per_mesh_materials = !materials.empty()
		&& materials.size() == object->meshes().size();

	bool texture_per_mesh = false;
	if (!per_mesh_materials)
	{
		if (object->textures().size() == object->meshes().size()) texture_per_mesh = true;
		else object->bind_textures();
	}

	const bool has_meshes = !object->meshes().empty();
	if (has_meshes)
	{
		auto model_meshes = object->meshes();
		int i = 0;
		for (const auto& mesh : model_meshes)
		{
			if (per_mesh_materials)
			{
				const auto& mat = materials.at(i);
				if (mat.diffuse)
				{
					mat.diffuse->bind(0);
					gl->set_uniform("has_texture", true);
				}
				else
				{
					gl->set_uniform("has_texture", false);
				}
				if (mat.specular)
				{
					mat.specular->bind(1);
					gl->set_uniform("gSpecularMap", 1);
				}
				gl->set_uniform("has_specular_map", mat.specular != nullptr);
			}
			else if (texture_per_mesh)
			{
				object->textures().at(i)->bind();
			}
			submit(shader, mesh, transform);
			i++;
		}
		if (per_mesh_materials)
			gl->set_uniform("has_specular_map", false);
	}
	else if (object->animated_mesh() != NULL)
	{
		object->animated_mesh()->on_render(transform, shader);
	}

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
}

void engine::renderer::submit(
	const ref<shader>& shader,
	const ref<skybox>& skybox,
	const glm::mat4& transform)
{
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("skybox_rendering", true);
	if (skybox->textures().size() == skybox->meshes().size())
	{
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
		uint32_t i = 0;
		auto model_meshes = skybox->meshes();
		for (const auto& mesh : model_meshes)
		{
			skybox->textures().at(i)->bind();
			submit(shader, mesh, transform);
			i++;
		}
		std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	}
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("skybox_rendering", false);
}
