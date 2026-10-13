#pragma once

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

namespace engine
{
    class mesh;
    class texture_2d;

    /// \brief
    class model final
    {
    public:
        model(const std::string& path);
        ~model();

		/// \brief Textures belonging to one mesh, as declared by its material.
		///
		/// Kept per mesh rather than in one flat list: which texture goes with
		/// which mesh used to be guessed by checking whether the texture count
		/// happened to equal the mesh count, which only worked when every mesh
		/// contributed exactly one texture. Any other model rendered with the
		/// wrong textures, which is why so few models "worked".
		struct mesh_material
		{
			ref<engine::texture_2d> diffuse;
			ref<engine::texture_2d> specular;
		};

		///\brief - Getters
		glm::vec3 size() const { return m_size; }
		glm::vec3 offset() const { return m_offset; }
		std::vector <ref<engine::mesh>> meshes() const { return m_meshes; }
		std::vector <ref<engine::texture_2d>> textures() const { return m_textures; }

		/// \brief Per-mesh materials, parallel to meshes() - same size, same order.
		const std::vector<mesh_material>& mesh_materials() const { return m_mesh_materials; }

		// create the object's smart pointer 
		static std::shared_ptr<engine::model> engine::model::create(const std::string& path);
		
    private: // methods
        void process_node(aiNode* node, const aiScene* scene);
        ref<engine::mesh> process_mesh(aiMesh* mesh, const aiScene* scene);
        std::vector<ref<texture_2d>> load_textures(aiMaterial* mat, aiTextureType type, const std::string& type_name);

        /// \brief Resolve one texture referenced by a material to a file on disk.
        ///
        /// Model files name their textures in whatever way the authoring tool
        /// happened to write them: bare filenames, paths with sub-directories,
        /// Windows or Unix separators, sometimes absolute paths from the
        /// artist's own machine. This tries the sensible candidates.
        ref<texture_2d> resolve_texture(const std::string& declared_path);
		
        ///\brief - compares the values of the vertex that is being loaded with min and max values
		/// if any coordinate is smaller than min or bigger than max, record it as new min/max value
		void min_max_compare(const glm::vec3& point);

    private: // fields
        std::string m_directory;
        std::string m_path;

		// model's meshes
		std::vector<ref<mesh>> m_meshes;

		// model's textures, flattened. Retained because lesson code and
		// game_object still consume a flat list; mesh_materials() is the
		// authoritative mapping.
		std::vector<ref<texture_2d>> m_textures;

		// one entry per mesh, in the same order as m_meshes
		std::vector<mesh_material> m_mesh_materials;

		// declared texture path -> loaded texture, so a texture shared by
		// several materials is loaded once. Per model, not static: a static
		// cache leaked textures between unrelated models.
		std::unordered_map<std::string, ref<texture_2d>> m_texture_cache;

		// min values of the model in x, y and z coordinates
		glm::vec3 m_min_point;

		// max values of the model in x, y and z coordinates
		glm::vec3 m_max_point;

		// switch that checks if the current point is the first point loaded for the model
		bool m_first_point = true;

		// dimensions of the object in x, y and z
		glm::vec3 m_size;

		// offset of the model from the origin of the local coordinates
		glm::vec3 m_offset;
    };
}
