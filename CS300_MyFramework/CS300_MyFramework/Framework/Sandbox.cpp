﻿/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Sandbox.cpp
Purpose: To provide a sandbox to play around with entities on screen
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "Sandbox.h"
#include "Renderer_D3D.h"

#include <glm/glm.hpp>

#include <minwinbase.h>
#include "Utils.h"
#include <minwinbase.h>
#include "ResourceManager.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include <ctime>
#include "Camera.h"
#include <glm/gtc/matrix_transform.inl>
#include "Window_DX.h"

#define PI 3.14159265359f

namespace
{
	Entity e1;
	Entity eFloor;
	Entity eSkyBox;

	Shader shader;

	Shader *skyShader;
	Shader *colorShader;
	Shader *reflShader;
	bool rotating = true;

	LightBufferData lightData;

	ID3D11Buffer *ProjectionBuffer;
	Camera cam(glm::vec3(0, 0, 1));
	
}

namespace Sandbox
{
	void initialize()
	{
		shader.loadPreCopiled("../Debug/", "PhongPix", true);
		colorShader = ResourceManager::loadShader("Color", "../Debug/");
		skyShader = ResourceManager::loadShader("SkyBox", "../Debug/");
		reflShader = ResourceManager::loadShader("ReflectionShader", "../Debug/");
		//colorShader.loadPreCopiled("../Debug/", "Color");
		Renderer_D3D::mapCBuffer(BUFFER_PROJECTION, 0, nullptr, SHADER_VERTEX | SHADER_GEOMETRY);
		Renderer_D3D::getDevContext()->RSSetState(Renderer_D3D::mRasterState);

		Texture2D *tex = ResourceManager::loadTexture("metal_roof_diff_512x512", "../textures/", ".tga");
		Texture2D *norm = ResourceManager::createNormalMap("metal_roof_spec_512x512", "../textures/", ".tga");
		//Texture2D *norm = ResourceManager::loadTexture("pillowNormal", "../textures/", ".png");
		Renderer_D3D::getDevContext()->PSSetShaderResources(0, 1, &tex->m_SRV);
		Renderer_D3D::getDevContext()->PSSetShaderResources(1, 1, &norm->m_SRV);


		eFloor.mMesh = ResourceManager::loadMesh("cube", "../models/cube.obj");
		eFloor.mPosition.z = -14;
		eFloor.mPosition.y = -14;
		eFloor.mScale = glm::vec3(20, 20, 20);

	
		e1.mMesh = ResourceManager::loadMesh("sphere", "../models/sphere.obj");//ResourceManager::loadMesh("sword", "../models/sword.obj");
		e1.mPosition.z = -5;
		e1.mPosition.y = -2;
		e1.mMaterial.diffuse = glm::vec4(0.25f, 0.25f, 0.25f, 1);
		e1.mMaterial.specular = glm::vec4(0.25f, 0.25f, 0.25f, 1);


		Texture2D *skyTex = ResourceManager::loadCubeMap("sky", "../textures/", ".jpg");
		
		Renderer_D3D::getDevContext()->PSSetShaderResources(3, 1, &skyTex->m_SRV);

		eSkyBox.mMesh = ResourceManager::getMesh("cube");
		eSkyBox.mScale = glm::vec3(1, 1, 1);
		eSkyBox.mPosition = glm::vec3(0, 0, 0);

		
		lightData.camPosition = glm::vec3(0, 0, 0);
		lightData.numLights = 8;
		lightData.globalAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
		lightData.falloff = 1;
		lightData.specIntesity = 100;
		lightData.fogColor = glm::vec3(0.0f, 0.2f, 0.4f);
		lightData.attCoeffs = glm::vec3(1, 0.1, 0);
		lightData.innerRadius = 15 * (PI / 180);
		lightData.outerRadius = 30 * (PI / 180);
		lightData.useTexture = false;

		for (int i = 0; i < lightData.numLights; ++i)
		{
			float r, g, b;
			r = ((rand() % 100) + 1) / 100.0f;
			g = ((rand() % 100) + 1) / 100.0f;
			b = ((rand() % 100) + 1) / 100.0f;
			float radius = 3;
			float angle = ((i + 1.0f) / lightData.numLights) * 2 * PI;

			lightData.lights[i].ambient = glm::vec4(0,0,0,0);
			lightData.lights[i].diffuse = glm::vec4(r, g, b, 1);
			lightData.lights[i].specular = glm::vec4(1,1,1,1);
			lightData.lights[i].direction = glm::vec4(0, 0, -1, 0);
			lightData.lights[i].position = glm::vec4(cos(angle) * radius, 0, sin(angle) * radius - 5, 0);
			lightData.lights[i].type = 2;
		}

	}

	void update()
	{
		glm::mat4x4 projMat = glm::perspectiveRH(45.0f, static_cast<float>(Window_DX::getWidth()) / Window_DX::getHeight() , 0.1f, 100.0f);
		//glm::mat4x4 projMat = glm::perspectiveRH(90.0f, 1.0f, 0.1f, 100.0f);
		glm::mat4 camMat = glm::transpose(cam.getTransform());
		glm::mat4 projData[2] =
		{
			camMat,
			projMat
		};
		Renderer_D3D::mapCBuffer(BUFFER_PROJECTION, sizeof(glm::mat4) * 2, projData, SHADER_VERTEX);

		Renderer_D3D::DrawEntity(eSkyBox, *skyShader);

		static float timer = 0;
		timer = clock() * 0.001f;

		if(rotating)
		{
			for (int i = 0; i < lightData.numLights; ++i)
			{
				float radius = 3;

				float angle = ((i + 1.0f) / lightData.numLights) * 2 * PI;

				lightData.lights[i].position = glm::vec4(cos(angle + timer) * radius, 0, sin(angle + timer) * radius - 5, 0);
				lightData.lights[i].direction = glm::normalize( glm::vec4(e1.mPosition.xyz, 1) - lightData.lights[i].position);
			}
			
			e1.mRotation.x += 0.001f;
			e1.mRotation.z += 0.001f;


		}


		ImGui_ImplDX11_NewFrame();

		ImGui::Begin("CS300_Framework");

		static char buff[256];
		ImGui::InputText("Model", buff, 256);
		if(ImGui::Button("Load Model"))
		{
			e1.mMesh = ResourceManager::loadMesh(buff, "../models/"+std::string(buff));
		}
		ImGui::Separator();
		static float yaw = -90;
		ImGui::SliderFloat("Yaw", &yaw, -180, 180);
		cam.setYaw(yaw);

		static float pitch = 0;
		ImGui::SliderFloat("Pitch", &pitch, -180, 180);
		cam.setPitch(pitch);

		ImGui::Separator();


		ImGui::Checkbox("Blinn", reinterpret_cast<bool *>(&lightData.useBlinn));
		ImGui::Checkbox("Rotating", &rotating);
		ImGui::Checkbox("UseTexture", reinterpret_cast<bool *>(&lightData.useTexture));

		ImGui::InputInt("Num Lights", &lightData.numLights);
		
		ImGui::SliderFloat3("Rotation", &e1.mRotation[0], 0, 6.28f); 
		static float scale = 1;
		ImGui::SliderFloat("Scale", &scale, 1, 20);
		for (int i = 0; i < 3; i++)
			e1.mScale[i] = scale;
		ImGui::InputFloat3("Position", &e1.mPosition[0]);
		ImGui::Checkbox("Draw Normals", &Renderer_D3D::m_RenderNormals);
		ImGui::Checkbox("Draw Tangents", &Renderer_D3D::m_RenderTangents);
		ImGui::Checkbox("Draw BiTangents", &Renderer_D3D::m_RenderBiTangents);
		ImGui::Checkbox("Use Normal Map as Tex", reinterpret_cast<bool*>(&Renderer_D3D::m_UseNormalMapAsTex));


		if(ImGui::CollapsingHeader("Material"))
		{
			ImGui::ColorEdit3("Ambient", &e1.mMaterial.ambient[0]);
			ImGui::ColorEdit3("Diffuse", &e1.mMaterial.diffuse[0]);
			ImGui::ColorEdit3("Specular", &e1.mMaterial.specular[0]);
		}

		ImGui::Separator();
		ImGui::InputFloat("NS", &lightData.specIntesity);
	
		for(int i = 0; i < lightData.numLights; i++)
		{
			char buffer[256];
			std::string name = "Light ";
			_itoa_s(i + 1, buffer, 10);
			name += buffer;
			ImGui::PushID(i);
			if(ImGui::CollapsingHeader(name.c_str()))
			{
				ImGui::InputFloat3("Direction", &lightData.lights[i].direction[0]);
				ImGui::InputFloat3("Position", &lightData.lights[i].position[0]);
				ImGui::ColorEdit4("Ambient", &lightData.lights[i].ambient[0]);
				ImGui::ColorEdit4("Diffuse", &lightData.lights[i].diffuse[0]);
				ImGui::ColorEdit4("Specular", &lightData.lights[i].specular[0]);
				
				int type = lightData.lights[i].type - 1;
				const char *types[] = { "Directional", "Point", "Spot" };
				ImGui::Combo("Type", &type, types, 3);
				lightData.lights[i].type = type + 1;
			}
			ImGui::PopID();

			Entity lightBall;
			lightBall.mPosition = lightData.lights[i].position;
			lightBall.mScale = glm::vec3(0.25f, 0.25f, 0.25f);
			lightBall.mColor = lightData.lights[i].diffuse;
			lightBall.mMesh = ResourceManager::loadMesh("sphere", "../models/sphere.obj");
			Renderer_D3D::DrawEntity(lightBall, *colorShader);
		}
		
		Renderer_D3D::setLightBuffer(lightData);
		Renderer_D3D::mapCBuffer(BUFFER_MATERIAL, sizeof(Material), &e1.mMaterial, SHADER_PIXEL);

		ImGui::End();
		
		Renderer_D3D::DrawEntity(e1, *reflShader);
		//Renderer_D3D::DrawEntity(eFloor, shader);

		Renderer_D3D::EndFrame();
	}
}
