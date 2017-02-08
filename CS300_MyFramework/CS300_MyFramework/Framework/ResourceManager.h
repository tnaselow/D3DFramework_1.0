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

class ResourceManager
{
	public:
		static Mesh *loadMesh(std::string name, std::string filePath);

		static Mesh *getMesh(std::string name);

	private:
		static std::map<std::string, Mesh> mMeshes;
		ResourceManager(){}
};
