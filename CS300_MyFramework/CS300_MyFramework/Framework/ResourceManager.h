/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ResourceManager.h
Purpose: To manager assets loaded from disc
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#pragma once
#include "Mesh.h"
#include <map>
#include "Texture2D.h"

class ResourceManager
{
	public:
		static Mesh *loadMesh(std::string name, std::string filePath);
		
		static Mesh *getMesh(std::string name);
		
		static Shader *loadShader(std::string name, std::string dir);
		static Shader *getShader(std::string name);

		static Texture2D *loadTexture(std::string name, std::string dir, std::string extension = ".png");
		static Texture2D *getTexture(std::string name);

		static Texture2D *loadCubeMap(std::string name, std::string dir, std::string extension = ".jpg", Texture2D *textures[6] = nullptr);

		static Texture2D *createNormalMap(std::string name, std::string dir, std::string extension = ".tga");

		static Texture2D *createTexture(std::string name, bool rtv, bool srv, unsigned width, unsigned height, char *data, bool dsv);

		static void cleanup();

	private:
		static std::map<std::string, Mesh> mMeshes;
		static std::map<std::string, Shader> mShaders;
		static std::map<std::string, Texture2D> m_Textures;
		ResourceManager() {}
};
