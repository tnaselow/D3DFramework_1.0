#pragma once


#include "assimp/config.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <vector>
#include "Texture2D.h"
#include <glm/glm.hpp>

class AssimpMesh;
class AssimpModel;

void processNode(AssimpModel &model, aiNode* node, const aiScene* scene);
AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene, std::string dir);

void setupMesh(AssimpMesh &mesh);
void loadModel(AssimpModel &model, std::string file);

struct AssimpVert
{
	glm::vec3 m_Position;
	glm::vec3 m_Normal;
	glm::vec2 m_TexCoord;
};

struct AssimpMesh
{
	std::vector<AssimpVert> m_Vertices;
	std::vector<uint32_t> m_Indices;
	std::vector<Texture2D *> m_Textures;

	ID3D11Buffer *m_VertBuffer;
	ID3D11Buffer *m_IndexBuffer;
	
	AssimpMesh(){}
	AssimpMesh(const std::vector<AssimpVert> &verts, const std::vector<uint32_t> &indices, const std::vector<Texture2D *> textures) :
				m_Vertices(verts), m_Indices(indices), m_Textures(textures)
	{
		setupMesh(*this);
	}
};


struct AssimpModel
{
	std::string directory;
	std::vector<AssimpMesh> m_Meshes;
};
