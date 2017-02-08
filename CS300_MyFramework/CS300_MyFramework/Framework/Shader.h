/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.h
Purpose: To create a wrapper around DX11 Shader functions
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#pragma once

#include "DX/d3d11.h"
#include "DX/d3dx11.h"
#include "DX/d3dx10.h"
#include <string>

enum ShaderType
{
	SHADER_PIXEL = 1,
	SHADER_VERTEX = 2,
	SHADER_GEOMETRY = 4
};

class Shader
{
	public:
		Shader() {};

		~Shader();
		void loadPreCopiled(const std::string &dir, const std::string &name, bool bind = false);
		void Bind(UINT type = SHADER_PIXEL | SHADER_VERTEX | SHADER_GEOMETRY);
		static void unBind(UINT type = SHADER_PIXEL | SHADER_VERTEX | SHADER_GEOMETRY);
		
	private:
		void loadShader(const std::string &file, ShaderType type);
		void loadInputElemnts(void *data, size_t size);
		ID3DBlob *mVS, *mPS;
		ID3D11VertexShader *mVertShader;
		ID3D11PixelShader  *mPixelShader;
		ID3D11GeometryShader *mGeometryShader;
		ID3D11InputLayout  *mInputLayout;
};
