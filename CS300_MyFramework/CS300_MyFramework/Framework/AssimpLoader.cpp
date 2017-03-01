#include "AssimpLoader.h"
#include "Utils.h"
#include "Renderer_D3D.h"
#include <iostream>

void setupMesh(AssimpMesh &mesh)
{
	// init vertex buffer
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(AssimpVert) * mesh.m_Vertices.size();
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA dataDesc;
	ZeroMemory(&dataDesc, sizeof(dataDesc));
	dataDesc.pSysMem = mesh.m_Vertices.data();

	HR(Renderer_D3D::getDevice()->CreateBuffer(&desc, &dataDesc, &mesh.m_VertBuffer));

	// init index buffer
	ZeroMemory(&desc, sizeof(desc));

	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(uint32_t) * mesh.m_Indices.size();
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	ZeroMemory(&dataDesc, sizeof(dataDesc));
	dataDesc.pSysMem = mesh.m_Indices.data();

	HR(Renderer_D3D::getDevice()->CreateBuffer(&desc, &dataDesc, &mesh.m_IndexBuffer));
}

void loadModel(AssimpModel &model, std::string file)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(file.c_str(), aiProcess_Triangulate);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Could not load Model at " << file << std::endl;
		return;
	}

	model.directory = file.substr(0, file.find_last_of('/'));
	processNode(model, scene->mRootNode, scene);
}


void processNode(AssimpModel &model, aiNode* node, const aiScene* scene)
{
	for (unsigned i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		model.m_Meshes.push_back(processMesh(mesh, scene));
	}
	for(unsigned i = 0; i < node->mNumChildren; ++i)
		processNode(model, node->mChildren[i], scene);
}

AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<AssimpVert> verts;
	std::vector<uint32_t> inds;
	std::vector<Texture2D> textures;

	for(unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		AssimpVert vert;
		glm::vec3 pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		glm::vec3 norm = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
		glm::vec2 texCoord;
		if (mesh->mTextureCoords[0])
			texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		else
			texCoord = { 0,0 };

		verts.push_back(vert);
	}

	for(unsigned i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned j = 0; j < face.mNumIndices; ++j)
			inds.push_back(face.mIndices[j]);
	}

	if(mesh->mMaterialIndex >= 0)
	{
		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
		//Texture2D diffuseMaps = loadT
	}

	return AssimpMesh(verts, inds, textures);
}