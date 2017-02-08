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

std::map<std::string, Mesh> ResourceManager::mMeshes;

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
	glm::vec4 averagePos(0,0,0,1);

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
			vert.Position.w = 1.0f;
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
		vertices[i].Position.w = 1.0f;
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
