/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.cpp
Purpose: To hold data about a 3D mesh including its vertex data and indices
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "Mesh.h"
#include "Utils.h"
#include "Renderer_D3D.h"

Mesh::~Mesh()
{
	SafeRelease(mVertBuffer);
	SafeRelease(mIndexBuffer);
}


void Mesh::setVerts(const std::vector<Vertex> &verts)
{
	mVertices = verts;
	setupVerts();
}
void Mesh::setIndices(const std::vector<UINT> &inds)
{
	mIndices = inds;
	setupIndices();
}

Mesh::Mesh(const std::vector<Vertex> &verts, const std::vector<UINT> &inds)
{
	mVertices = verts;
	mIndices = inds;

	setupVerts();
	setupIndices();
}

void Mesh::setupIndices()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&subData, sizeof(subData));
	
	// setup index buffer
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = sizeof(unsigned) * mIndices.size();

	subData.pSysMem = mIndices.data();
	HR(Renderer_D3D::getDevice()->CreateBuffer(&bd, &subData, &mIndexBuffer));
}

void Mesh::setupVerts()
{
	// setup vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_IMMUTABLE;                 
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       
	bd.CPUAccessFlags = 0;    
	bd.ByteWidth = sizeof(Vertex) * mVertices.size(); 

	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&subData, sizeof(subData));
	subData.pSysMem = mVertices.data();

	HR(Renderer_D3D::getDevice()->CreateBuffer(&bd, &subData, &mVertBuffer)); 
}
