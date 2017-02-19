﻿/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Sandbox.h
Purpose: To provide a sandbox to play around with entities on screen
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#pragma once
#define GLM_FORCE_SWIZZLE 
#include "glm/glm.hpp"
#include "Mesh.h"

struct Light
{
	glm::vec4 position;
	glm::vec4 direction;
			
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	// TYPES
	// 1 - dir
	// 2 - point
	// 3 - spot
	unsigned type; 
	glm::vec3 allign;
};

struct LightBufferData
{
	Light lights[8];

	int numLights;
	glm::vec3 globalAmbient;

	float innerRadius;
	float outerRadius;
	float falloff;
	float specIntesity;

	glm::vec3 attCoeffs;
	int useBlinn;

	glm::vec3 fogColor;
	int useTexture;

	glm::vec3 camPosition;
	float allign_4;
};

struct Material
{
	Material() : ambient(0.1f, 0.1f, 0.1f, 1.0f),diffuse(1,1,1,1), specular(1,1,1,1){}
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
};

struct Entity
{
	Entity() : mPosition(0,0,0), mRotation(0,0,0), mScale(1,1,1), mColor(1,1,1,1), mMesh(nullptr) {}
	glm::vec3 mPosition;
	glm::vec3 mRotation;
	glm::vec3 mScale;

	glm::vec4 mColor;

	Material mMaterial;

	Mesh *mMesh;
};

namespace Sandbox
{
	void initialize();
	void update();
}
