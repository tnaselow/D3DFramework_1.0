/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ResourceManager.cpp
Purpose: To manager assets loaded from disc
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "ResourceManager.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include "Utils.h"
#include <iostream>
#include "Renderer_D3D.h"
#define PI 3.14159265359f
#define EPSILON 0.000001f

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::map<std::string, Mesh> ResourceManager::mMeshes;
std::map<std::string, Shader> ResourceManager::mShaders;
std::map<std::string, Texture2D> ResourceManager::m_Textures;

float heightLookup(int x, int y, unsigned char *data, unsigned stride)
{
	return data[x * 4 + y * stride] / 255.0f;
}

Texture2D *ResourceManager::createNormalMap(std::string name, std::string dir, std::string extension /*= ".tga"*/)
{
	auto iter = m_Textures.find(name);
	if (iter != m_Textures.end())
		return &m_Textures[name];

	// loading texture data
	int width, height;
	// maybe do something with this
	int numChannels;
	unsigned char *image = stbi_load((dir + name + extension).c_str(), &width, &height, &numChannels, 0);

	if (image == nullptr)
		std::cout << "couldnt load image " + name << std::endl;

	unsigned char *normalMap = new unsigned char[width * height * 4];
	unsigned char *fourChannelImage = new unsigned char[width * height * 4];

	//TODO: expose this to imgui
	float scalar = 0.5;

	unsigned stride = width * 4;
	if(numChannels < 4)
	{
		for(unsigned y = 0; y < height; ++y)
		{
			for(unsigned x = 0; x < width; ++x)
			{
				fourChannelImage[(x * 4 + y * stride)    ] = image[x * 3 + y * width * 3    ];
				fourChannelImage[(x * 4 + y * stride) + 1] = image[x * 3 + y * width * 3 + 1];
				fourChannelImage[(x * 4 + y * stride) + 2] = image[x * 3 + y * width * 3 + 2];
				fourChannelImage[(x * 4 + y * stride) + 3] = 255;			}
		}		
	}
	else
	{
		fourChannelImage = image;
	}


	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			int xplus1 = x + 1;
			int xminus1 = x - 1;
			if (xplus1 == width)
				xplus1 = x;
			if (xminus1 < 0)
				xminus1 = 0;

			glm::vec3 S = glm::vec3(1, 0, scalar * heightLookup(xplus1, y, fourChannelImage, stride) -
										  scalar * heightLookup(xminus1, y, fourChannelImage, stride));
			
			int yplus1 = y + 1;
			int yminus1 = y - 1;
			if (yplus1 == height)
				yplus1 = y;
			if (yminus1 < 0)
				yminus1 = 0;

			glm::vec3 T = glm::vec3(0, 1, scalar * heightLookup(x, yplus1, fourChannelImage, stride) -
										  scalar * heightLookup(x, yminus1, fourChannelImage, stride));

			glm::vec3 N = glm::cross(S, T);
			//N.y *= -1;
			N = glm::normalize(N);
			// map normal to range of 0-1
			N += 1;
			N /= 2;

			normalMap[x * 4 + y * stride    ] = N.x * 255;
			normalMap[x * 4 + y * stride + 1] = N.y * 255;
			normalMap[x * 4 + y * stride + 2] = N.z * 255;
			normalMap[x * 4 + y * stride + 3] = 255;
		}
	}

	// setting up d3d texture
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = normalMap;
	data.SysMemPitch = width * 4; // stride of texture

	ID3D11Texture2D *texture;
	HR(Renderer_D3D::getDevice()->CreateTexture2D(&textureDesc, &data, &texture));
	HR(Renderer_D3D::getDevice()->CreateShaderResourceView(texture, &srvd, &m_Textures[name].m_SRV));

	m_Textures[name].m_Height = height;
	m_Textures[name].m_Width = width;
	m_Textures[name].m_Name = name;

	SafeRelease(texture);
	delete[] normalMap;
	stbi_image_free(image);

	return &m_Textures[name];

}

Texture2D *ResourceManager::loadTexture(std::string name, std::string dir, std::string extension)
{
	auto iter = m_Textures.find(name);
	if (iter != m_Textures.end())
		return &m_Textures[name];

	// loading texture data
	int width, height;
	// maybe do something with this
	int numChannels;
	unsigned char *image = stbi_load((dir+name+extension).c_str(), &width, &height, &numChannels, 0);
	unsigned char *fourChannelImage = new unsigned char[width * height * 4];
	int stride = width * 4;

	if (image == nullptr)
		std::cout << "couldnt load image " + name << std::endl;
	
	if (numChannels < 4)
	{
		for (unsigned y = 0; y < height; ++y)
		{
			for (unsigned x = 0; x < width; ++x)
			{
				fourChannelImage[(x * 4 + y * stride)] = image[x * 3 + y * width * 3];
				fourChannelImage[(x * 4 + y * stride) + 1] = image[x * 3 + y * width * 3 + 1];
				fourChannelImage[(x * 4 + y * stride) + 2] = image[x * 3 + y * width * 3 + 2];
				fourChannelImage[(x * 4 + y * stride) + 3] = 255;
			}
		}
	}
	else
	{
		fourChannelImage = image;
	}

	// setting up d3d texture
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = fourChannelImage;
	data.SysMemPitch = stride; // stride of texture

	ID3D11Texture2D *texture;
	HR(Renderer_D3D::getDevice()->CreateTexture2D(&textureDesc, &data, &texture));
	HR(Renderer_D3D::getDevice()->CreateShaderResourceView(texture, &srvd, &m_Textures[name].m_SRV));

	m_Textures[name].m_Height = height;
	m_Textures[name].m_Width = width;
	m_Textures[name].m_Name = name;
	
	SafeRelease(texture);
	stbi_image_free(image);
	delete[] fourChannelImage;

	return &m_Textures[name];
}

Texture2D *ResourceManager::getTexture(std::string name)
{
	auto iter = m_Textures.find(name);
	assert(iter != m_Textures.end());
	return &m_Textures[name];
	
}

Shader *ResourceManager::loadShader(std::string name, std::string dir)
{
	auto it = mShaders.find(name);
	if (it != mShaders.end())
		return &mShaders[name];

	mShaders[name].loadPreCopiled(dir, name);
	return &mShaders[name];
}
Shader *ResourceManager::getShader(std::string name)
{
	auto it = mShaders.find(name);
	if (it != mShaders.end())
		return &mShaders[name];
}


Mesh *ResourceManager::loadMesh(std::string name, std::string filePath)
{
	// if the mesh is already loaded return it
	auto it = mMeshes.find(name);
	if (it != mMeshes.end())
		return &mMeshes[name];

	// open file
	std::ifstream inFile;
	inFile.open(filePath);
	assert(inFile.is_open() == true);

	// containers used to load in data
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
	std::vector<unsigned> indices;
	std::vector<std::vector<Face>> adjFaces;

	std::string word;
	glm::vec3 averagePos(0,0,0);

	while(inFile >> word)
	{
		// if this line is a vertex add the values to
		// vertices vector
		if (word == "v")
		{
			Vertex vert;
			inFile >> word;
			vert.Position.x = atof(word.c_str());
			inFile >> word;
			vert.Position.y = atof(word.c_str());
			inFile >> word;
			vert.Position.z = atof(word.c_str());
			//vert.Position.w = 1.0f;
			vertices.push_back(vert);
			
			// also add this vertex position to 
			// the average position of all verts (used for centering)
			averagePos += vert.Position;
		}
		// if this line represnets a face
		// load in each index into the indices vector
		// once all the indices have been loaded in add the face to the face vector
		else if (word == "f")
		{
			Face face;
			inFile >> word;
			face.f1 = atoi(word.c_str()) - 1;	
			indices.push_back(face.f1);
			inFile >> word;
			face.f2 = atoi(word.c_str()) - 1;
			indices.push_back(face.f2);
			inFile >> word;
			face.f3 = atoi(word.c_str()) - 1;
			indices.push_back(face.f3);
			faces.push_back(face);
		}
	}

	
	// add a list of adjacent faces for each vertex
	adjFaces.resize(vertices.size());
	for(int i = 0; i < faces.size(); ++i)
	{
		adjFaces[faces[i].f1].push_back(faces[i]);
		adjFaces[faces[i].f2].push_back(faces[i]);
		adjFaces[faces[i].f3].push_back(faces[i]);
	}

	float length = 0;
	averagePos /= vertices.size();
	for (int i = 0; i < vertices.size(); ++i)
	{
		// center each point around the origin
		vertices[i].Position -= averagePos;
		// find the farthest away vert
		float tempLength = glm::length(vertices[i].Position);
		if (tempLength > length)
			length = tempLength;
	}
	
	//normalize all positions and set w = 1 to make it a point
	for (int i = 0; i < vertices.size(); ++i)
	{
		vertices[i].Position /= length;
		//vertices[i].Position.w = 1.0f;
	}

	// loop over all vertices and find their normals using adjacent faces
	for(int i = 0; i < vertices.size(); ++i)
	{
		glm::vec3 normal(0,0,0);
		for(int j = 0; j < adjFaces[i].size(); j++)
		{
			glm::vec3 faceNormal;
			glm::vec3 b = vertices[adjFaces[i][j].f1].Position;
			glm::vec3 a = vertices[adjFaces[i][j].f2].Position;
			glm::vec3 c = vertices[adjFaces[i][j].f3].Position;
			glm::vec3 ab, ac;
			ab = b - a;
			ac = c - a;
			faceNormal = glm::cross(ac, ab);
			normal += faceNormal;
		}

		vertices[i].Normal = glm::vec4(glm::normalize(normal), 0);
	}

	// calculate tex coords
	for(unsigned i = 0; i < vertices.size(); ++i)
	{
		// spherical coords
		float r = glm::length(vertices[i].Position);
		float temp = atan(vertices[i].Position.z / vertices[i].Position.x);
		float temp2 = tan(vertices[i].Position.y / r);
		vertices[i].texCoords = glm::vec2((temp + PI) / (2.0f * PI), (temp2 + PI) / (2.0f * PI));

		// cubular coords
		//glm::vec3 mag = glm::abs(vertices[i].Position);
		//glm::vec3 biasUVs = glm::vec3(0.5f) + 0.5f * vertices[i].Position;
		//if (mag.x > mag.y && mag.x > mag.z)
		//	vertices[i].texCoords = glm::vec2(biasUVs.y, biasUVs.z);
		//else if(mag.y > mag.z)
		//	vertices[i].texCoords = glm::vec2(biasUVs.x, biasUVs.z);
		//else 
		//	vertices[i].texCoords = glm::vec2(biasUVs.x, biasUVs.y);
	}

	// calculate tans and bitans
	for (unsigned i = 0; i < vertices.size(); ++i)
	{
		glm::vec3 tangent;
		glm::vec3 biTangent;
		float tangentMag = 0;
		float biTangentMag = 0;

		for(unsigned j = 0; j < adjFaces[i].size(); ++j)
		{
			glm::vec3 faceTangent;
			glm::vec3 faceBiTangent;

			glm::vec3 e1 = vertices[adjFaces[i][j].f2].Position - vertices[adjFaces[i][j].f1].Position;
			glm::vec3 e2 = vertices[adjFaces[i][j].f3].Position - vertices[adjFaces[i][j].f1].Position;
			glm::vec2 deltaUV1 = vertices[adjFaces[i][j].f2].texCoords - vertices[adjFaces[i][j].f1].texCoords;
			glm::vec2 deltaUV2 = vertices[adjFaces[i][j].f3].texCoords - vertices[adjFaces[i][j].f1].texCoords;

			float d = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
			if (abs(d) < EPSILON)
			{
				faceTangent.x = 1.0f;
				faceTangent.y = 0.0f;
				faceTangent.z = 0.0f;

				faceBiTangent.x = 0.0f;
				faceBiTangent.y = 0.0f;
				faceBiTangent.z = 1.0f;
			}
			else
			{
				float f = 1.0f / d;
				faceTangent.x = f * (deltaUV2.y * e1.x - deltaUV1.y * e2.x);
				faceTangent.y = f * (deltaUV2.y * e1.y - deltaUV1.y * e2.y);
				faceTangent.z = f * (deltaUV2.y * e1.z - deltaUV1.y * e2.z);

				faceBiTangent.x = f * (-deltaUV2.x * e1.x + deltaUV1.x * e2.x);
				faceBiTangent.y = f * (-deltaUV2.x * e1.y + deltaUV1.x * e2.y);
				faceBiTangent.z = f * (-deltaUV2.x * e1.z + deltaUV1.x * e2.z);
			}

			tangent += faceTangent;
			biTangent += faceBiTangent;
			tangentMag += faceTangent.length();
			biTangentMag += faceBiTangent.length();
		}

		assert(!isnan(tangent.x));
		tangent = glm::normalize(tangent) * (tangentMag / adjFaces[i].size());
		biTangent = glm::normalize(biTangent) * (biTangentMag / adjFaces[i].size());
		vertices[i].Tangent = tangent;
		vertices[i].BiTangent = biTangent;
	}
	
	// add mesh to resource manager and return it
	mMeshes[name].setVerts(vertices);
	mMeshes[name].setIndices(indices);
	return &mMeshes[name];
}

Mesh *ResourceManager::getMesh(std::string name)
{
	auto it = mMeshes.find(name);
	assert(it != mMeshes.end());

	return &mMeshes[name];
}

void ResourceManager::cleanup()
{
	// TODO : in the next iteration of the engine have the resource manager
	// responsible for cleaning up all assets and not the assets cleaning themselves up
	for (auto tex : m_Textures)
		SafeRelease(tex.second.m_SRV);
}