/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: To hold data about a 3D mesh including its vertex data and indices
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#pragma once

#include "DX/D3D11.h"
#include "dx/D3DX11.h"
#include  "dx/d3dx10.h"
#include <vector>
#include "Shader.h"
#include "glm/glm.hpp"

struct Vertex
{
	glm::vec3 Position; // only use vec3
	glm::vec3 Normal;   // only use vec3
	glm::vec2 texCoords;

	glm::vec3 Tangent;
	glm::vec3 BiTangent;
};

struct Face
{
	unsigned f1, f2, f3;
	//glm::vec3 tangent;
	//glm::vec
};


class Mesh
{
	public:
		std::vector<Vertex> mVertices;
		std::vector<UINT> mIndices;
		std::vector<glm::vec3> mFacePos;

		Mesh(const std::vector<Vertex> &verts, const std::vector<UINT> &inds);
		Mesh() : mVertBuffer(nullptr), mIndexBuffer(nullptr) {}
		~Mesh();

		void setVerts(const std::vector<Vertex> &verts);
		void setIndices(const std::vector<UINT> &inds);
		void setShader(const Shader &shader);

		ID3D11Buffer *mVertBuffer;
		ID3D11Buffer *mIndexBuffer;
	private:
		void setupVerts();
		void setupIndices();
		
};

