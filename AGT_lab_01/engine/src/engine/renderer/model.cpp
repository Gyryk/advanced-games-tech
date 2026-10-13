#include "pch.h"
#include "model.h"
#include "mesh.h"
#include "renderer.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


engine::model::model(const std::string& path) : m_path (path)
{
    LOG_CORE_INFO("[model] Creating model '{0}'.", m_path);
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(m_path,
        aiProcess_Triangulate
        | aiProcess_FlipUVs
        // Without this, a model that ships no normals renders unlit. Assimp
        // only generates them if asked, and plenty of exported models omit
        // them. GenSmoothNormals is a no-op when normals are already present.
        | aiProcess_GenSmoothNormals
        // Needed for normal mapping; fills mTangents.
        | aiProcess_CalcTangentSpace);

    if((!scene) || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode))
    {
        // importer.GetErrorString() says *why*; without it a failed load was
        // indistinguishable from a missing file.
        LOG_CORE_ERROR("[model] Unable to load model '{0}': {1}", m_path,
            importer.GetErrorString());
        return;
    }
    // Split on both separators. With '/' alone a Windows-style path finds no
    // match, find_last_of returns npos, npos + 1 wraps to 0, and substr(0, 0)
    // yields an empty directory - so every texture was then looked for
    // relative to the working directory instead of next to the model.
    const size_t slash = m_path.find_last_of("/\\");
    m_directory = (slash == std::string::npos) ? "" : m_path.substr(0, slash + 1);

    process_node(scene->mRootNode, scene);

	m_offset = (m_max_point + m_min_point) / 2.0f;
	m_size = m_max_point - m_min_point;
}

engine::model::~model()
{
    LOG_CORE_INFO("[model] Destroying model '{0}'.", m_path);
}

void engine::model::process_node(aiNode* node, const aiScene* scene)
{
    for(uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(process_mesh(mesh, scene));
    }

    for(uint32_t i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene);
    }
}

engine::ref<engine::mesh> engine::model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<mesh::vertex> vertices;
    std::vector<uint32_t> indices;

    // == Process vertices
    for(uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        mesh::vertex vert{};

        // Position
        glm::vec3 pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vert.position = pos;

		if (m_first_point)
		{
			m_min_point = pos; m_max_point = pos;
			m_first_point = false;
		}
		else
		{
			min_max_compare(pos);
		}

        // Normal. GenSmoothNormals guarantees mNormals, but guard anyway -
        // dereferencing it unconditionally crashed on meshes without normals.
        if (mesh->HasNormals())
            vert.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        else
            vert.normal = glm::vec3(0.f, 1.f, 0.f);

        // Tangent, for normal mapping. Absent if the mesh has no UVs, in which
        // case it stays zero and the shader uses the vertex normal instead.
        if (mesh->HasTangentsAndBitangents())
            vert.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

        // TexCoords
        if(mesh->mTextureCoords[0])
        {// Does it have any texture coordinates?
            glm::vec2 tex(mesh->mTextureCoords[0][i].x, 1.f - mesh->mTextureCoords[0][i].y);
            vert.tex_coords = tex;
        }
        else
        {
            vert.tex_coords = glm::vec2(0.0f, 0.0f);
        }

        // Push into vertex array
        vertices.push_back(vert);
    }

    // == Process indices
    for(uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(uint32_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // == Process materials/textures
    //
    // Recorded per mesh via mMaterialIndex, which assimp has been telling us
    // all along. The flat m_textures list is still filled for the benefit of
    // existing callers, but mesh_materials() is what the renderer should use:
    // a flat list cannot express "these two meshes share one texture" or
    // "this mesh has a diffuse and a specular map", and guessing from counts
    // silently mis-paired both cases.
    mesh_material mat_textures{};

    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<ref<texture_2d>> diffuse_maps = load_textures(material, aiTextureType_DIFFUSE, "diffuse");
        std::vector<ref<texture_2d>> specular_maps = load_textures(material, aiTextureType_SPECULAR, "specular");

        // The shader binds one of each, so take the first if there are several.
        if (!diffuse_maps.empty())  mat_textures.diffuse = diffuse_maps.front();
        if (!specular_maps.empty()) mat_textures.specular = specular_maps.front();

        m_textures.insert(m_textures.end(), diffuse_maps.begin(), diffuse_maps.end());
        m_textures.insert(m_textures.end(), specular_maps.begin(), specular_maps.end());

        if (diffuse_maps.empty() && specular_maps.empty())
        {
            // No texture at all: stand in a 1x1 texture of the material's
            // diffuse colour so the mesh is not rendered untextured.
            aiColor3D color(0.0f, 0.0f, 0.0f);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

            BYTE data[3];
            data[0] = (BYTE)(color[0] * 255);
            data[1] = (BYTE)(color[1] * 255);
            data[2] = (BYTE)(color[2] * 255);
            // NB: create() takes b, g, r in that order.
            mat_textures.diffuse = engine::texture_2d::create(data[2], data[1], data[0]);
            m_textures.push_back(mat_textures.diffuse);
        }
    }

    m_mesh_materials.push_back(mat_textures);
    return mesh::create(vertices, indices);
}

engine::ref<engine::texture_2d> engine::model::resolve_texture(const std::string& declared_path)
{
    if (declared_path.empty())
        return nullptr;

    // Shared by several materials? Reuse it.
    //
    // The old cache compared the *loaded* texture's full path against the raw
    // path from the model file. Those never matched, so nothing was ever
    // reused and the (static, process-wide) list grew forever. Key on the
    // declared path, which is what we are actually given.
    const auto cached = m_texture_cache.find(declared_path);
    if (cached != m_texture_cache.end())
        return cached->second;

    // Normalise separators so sub-directory paths work whichever way the
    // exporter wrote them.
    std::string relative = declared_path;
    std::replace(relative.begin(), relative.end(), '\\', '/');

    // Strip any leading "./"
    if (relative.rfind("./", 0) == 0)
        relative = relative.substr(2);

    const size_t slash = relative.find_last_of('/');
    const std::string basename =
        (slash == std::string::npos) ? relative : relative.substr(slash + 1);

    // Candidates, most-specific first. Exporters write texture paths in all of
    // these shapes, including absolute paths from the artist's own machine,
    // which is why the basename fallbacks matter.
    const std::vector<std::string> candidates = {
        m_directory + relative,              // as declared, next to the model
        m_directory + basename,              // sub-directories flattened
        m_directory + "textures/" + basename,// a conventional sub-folder
        relative,                            // absolute, or relative to cwd
        basename,
    };

    for (const auto& candidate : candidates)
    {
        ref<texture_2d> tex = texture_2d::create(candidate, false);
        if (tex && tex->loaded())
        {
            m_texture_cache[declared_path] = tex;
            return tex;
        }
    }

    // Not found under its own name: try the same name with the other common
    // image extensions. Model files often name a .bmp or .tga that was later
    // converted (newSoccerBall.3ds asks for SOCCER_T.BMP; the file is
    // SOCCER_T.png). The old loader handled this by rewriting the extension to
    // png; this keeps that working without guessing silently.
    static const char* const k_extensions[] = { ".png", ".jpg", ".jpeg", ".bmp", ".tga" };
    for (const auto& candidate : candidates)
    {
        const size_t dot = candidate.find_last_of('.');
        const size_t sep = candidate.find_last_of('/');
        if (dot == std::string::npos || (sep != std::string::npos && dot < sep))
            continue;
        const std::string stem = candidate.substr(0, dot);
        for (const char* ext : k_extensions)
        {
            const std::string alt = stem + ext;
            if (alt == candidate)
                continue;
            ref<texture_2d> tex = texture_2d::create(alt, false);
            if (tex && tex->loaded())
            {
                LOG_CORE_WARN("[model] '{0}': texture '{1}' not found; using '{2}'.",
                              m_path, declared_path, alt);
                m_texture_cache[declared_path] = tex;
                return tex;
            }
        }
    }

    // Report it rather than failing silently. Previously an unresolved texture
    // produced a black or wrongly-textured mesh with nothing in the log, and
    // the extension was rewritten to "png" if it was not on a hardcoded list -
    // so a .dds or .tif reference became a filename that could never exist.
    LOG_CORE_ERROR("[model] '{0}': texture '{1}' not found. Tried '{2}' and "
                   "'{3}'. Place it next to the model, or in a textures/ "
                   "sub-folder.",
                   m_path, declared_path,
                   m_directory + relative, m_directory + basename);
    m_texture_cache[declared_path] = nullptr;
    return nullptr;
}

std::vector<engine::ref<engine::texture_2d>> engine::model::load_textures(aiMaterial* mat, aiTextureType type, const std::string& type_name)
{
    std::vector<ref<texture_2d>> textures;
    for(uint32_t i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString filename;
        if (mat->GetTexture(type, i, &filename) != AI_SUCCESS)
            continue;

        if (ref<texture_2d> tex = resolve_texture(filename.C_Str()))
            textures.push_back(tex);
        else
            LOG_CORE_WARN("[model] '{0}': skipping {1} map '{2}'.",
                          m_path, type_name, filename.C_Str());
    }

    return textures;
}

// TODO - objects (any object for now, more edge cases will be explained on the way) should be passed as !!! const& !!!
void engine::model::min_max_compare(const glm::vec3& point)
{
	if (point.x < m_min_point.x)
		m_min_point.x = point.x;
	if (point.x > m_max_point.x)
		m_max_point.x = point.x;
	if (point.y < m_min_point.y)
		m_min_point.y = point.y;
	if (point.y > m_max_point.y)
		m_max_point.y = point.y;
	if (point.z < m_min_point.z)
		m_min_point.z = point.z;
	if (point.z > m_max_point.z)
		m_max_point.z = point.z;
}

std::shared_ptr<engine::model> engine::model::create(const std::string& path)
{
	return std::make_shared<engine::model>(path);
}
