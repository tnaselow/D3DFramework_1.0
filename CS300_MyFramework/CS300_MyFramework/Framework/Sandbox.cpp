/* Start Header -------------------------------------------------------
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

#define PI 3.14159265359f

namespace
{
	Entity e1;
	Entity eFloor;
	Shader shader;
	Shader colorShader;

	LightBufferData lightData;

	ID3D11Buffer *ProjectionBuffer;
}

namespace Sandbox
{
	void initialize()
	{
		shader.loadPreCopiled("../Debug/", "PhongPix", true);
		colorShader.loadPreCopiled("../Debug/", "Color");
		//Renderer_D3D::getDevContext()->VSSetConstantBuffers(0, 1, &Renderer_D3D::mProjBuffer);
		//Renderer_D3D::getDevContext()->GSSetConstantBuffers(0, 1, &Renderer_D3D::mProjBuffer);
		Renderer_D3D::mapCBuffer(BUFFER_PROJECTION, 0, nullptr, SHADER_VERTEX | SHADER_GEOMETRY);
		Renderer_D3D::getDevContext()->RSSetState(Renderer_D3D::mRasterState);
	
		e1.mMesh = ResourceManager::loadMesh("sword", "../models/sword.obj");
		e1.mPosition.z = -5;

		eFloor.mMesh = ResourceManager::loadMesh("cube", "../models/cube.obj");
		eFloor.mPosition.z = -14;
		eFloor.mPosition.y = -14;
		eFloor.mScale = glm::vec3(20, 20, 20);

		
		lightData.numLights = 2;
		lightData.globalAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
		lightData.falloff = 1;
		lightData.specIntesity = 1;
		lightData.fogColor = glm::vec3(0.0f, 0.2f, 0.4f);
		lightData.attCoeffs = glm::vec3(1, 0.1, 0);
		lightData.innerRadius = 15 * (PI / 180);
		lightData.outerRadius = 30 * (PI / 180);

		for (int i = 0; i < lightData.numLights; ++i)
		{
			lightData.lights[i].ambient = glm::vec4(0,0,0,0);
			lightData.lights[i].diffuse = glm::vec4(0.8, 0.8, 0.8, 1);
			lightData.lights[i].specular = glm::vec4(1,1,1,1);
			lightData.lights[i].direction = glm::vec4(0, 0, -1, 0);
			lightData.lights[i].position = glm::vec4(0, 0, 0, 0);
			lightData.lights[i].type = 3;
		}

		lightData.lights[1].type = 2;
		lightData.lights[1].position = glm::vec4(5, 5, -5, 1);
	}

	void update()
	{
		ImGui_ImplDX11_NewFrame();

		ImGui::Begin("CS300_Framework");

		static char buff[256];
		ImGui::InputText("Model", buff, 256);
		if(ImGui::Button("Load Model"))
		{
			e1.mMesh = ResourceManager::loadMesh(buff, "../models/"+std::string(buff));
		}
		ImGui::Separator();

		ImGui::Checkbox("Blinn", reinterpret_cast<bool *>(&lightData.useBlinn));
		
		ImGui::SliderFloat3("Rotation", &e1.mRotation[0], 0, 6.28f); 
		static float scale = 1;
		ImGui::SliderFloat("Scale", &scale, 1, 20);
		for (int i = 0; i < 3; i++)
			e1.mScale[i] = scale;
		ImGui::InputFloat3("Position", &e1.mPosition[0]);
		if(ImGui::CollapsingHeader("Material"))
		{
			ImGui::ColorEdit3("Ambient", &e1.mMaterial.ambient[0]);
			ImGui::ColorEdit3("Diffuse", &e1.mMaterial.diffuse[0]);
			ImGui::ColorEdit3("Specular", &e1.mMaterial.specular[0]);
		}

		ImGui::Separator();
	
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
			Renderer_D3D::DrawEntity(lightBall, colorShader);
		}
		
		Renderer_D3D::setLightBuffer(lightData);
		Renderer_D3D::mapCBuffer(BUFFER_MATERIAL, sizeof(Material), &e1.mMaterial, SHADER_PIXEL);

		ImGui::End();
		
		Renderer_D3D::DrawEntity(e1, shader);
		Renderer_D3D::DrawEntity(eFloor, shader);

		Renderer_D3D::EndFrame();
	}
}
