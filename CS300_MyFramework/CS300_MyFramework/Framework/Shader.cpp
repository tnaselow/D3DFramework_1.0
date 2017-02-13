/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.cpp
Purpose: To create a wrapper around DX11 Shader functions
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "Shader.h"
#include "Utils.h"
#include <fstream>
#include "Renderer_D3D.h"
#include <DX/d3d11shader.h>
#include <DX/d3dcompiler.h>
#include <vector>
#include <iostream>

#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)

void Shader::loadPreCopiled(const std::string &dir, const std::string &name, bool bind)
{
	std::string vsPath = dir + name + "_VS.cso";
	std::string psPath = dir + name + "_PS.cso";
	std::string gsPath = dir + name + "_G.cso";
	
	loadShader(vsPath, ShaderType::SHADER_VERTEX, true);
	loadShader(psPath, ShaderType::SHADER_PIXEL, true);
	loadShader(gsPath, ShaderType::SHADER_GEOMETRY, true);
	
	if (bind) Bind();
}

void Shader::loadShaderFromFile(const std::string &dir, const std::string &name, bool bind)
{
	std::string vsPath = dir + name + "_VS.hlsl";
	std::string psPath = dir + name + "_PS.hlsl";
	std::string gsPath = dir + name + "_G.hlsl";

	loadShader(vsPath, ShaderType::SHADER_VERTEX, false);
	loadShader(psPath, ShaderType::SHADER_PIXEL, false);
	loadShader(gsPath, ShaderType::SHADER_GEOMETRY, false);

	if (bind) Bind();
}

void Shader::loadShader(const std::string &file, ShaderType type, bool preCompiled)
{
	std::ifstream in;
	in.open(file, std::ifstream::in | std::ifstream::binary);

	// test for open
	if (!in.is_open())
	{
		std::cout << "could not load shader "+ file<< std::endl;
		return;
	}

	void *data = nullptr;
	size_t size = 0;

	if (preCompiled)
	{
		in.seekg(0, std::ios::end);
		size = size_t(in.tellg());
		data = new char[size];
		in.seekg(0, std::ios::beg);
		in.read(reinterpret_cast<char*>(data), size);
		in.close();
	}
	//else
	//{
	//	ID3DBlob *dataBlob;
	//	ID3DBlob *errorBlob;
	//
	//	std::string fLevel;
	//	switch (type)
	//	{
	//	case SHADER_PIXEL:
	//		fLevel = "ps_";
	//		break;
	//	case SHADER_VERTEX:
	//		fLevel = "vs_";
	//		break;
	//	case SHADER_GEOMETRY:
	//		fLevel = "gs_";
	//		break;
	//	}
	//	fLevel += "4_1";
	//	std::wstring wString(file.begin(), file.end());
	//	HRESULT hr = 0;
	//	
	//	D3DX11CompileFromFile(wString.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
	//										nullptr, fLevel.c_str(), 0, 0, nullptr, &dataBlob, &errorBlob, &hr);
	//	if (FAILED(hr))
	//	{
	//		std::cout << reinterpret_cast<char *>(errorBlob->GetBufferPointer()) << std::endl;
	//		assert(0);
	//	}
	//	data = dataBlob->GetBufferPointer();
	// 	size = dataBlob->GetBufferSize();
	//}

	switch(type)
	{
		case ShaderType::SHADER_VERTEX:
			Renderer_D3D::getDevice()->CreateVertexShader(data, size, NULL, &mVertShader);
			loadInputElemnts(data, size);
		break;
			
		case ShaderType::SHADER_PIXEL:
			Renderer_D3D::getDevice()->CreatePixelShader(data, size, NULL, &mPixelShader);
		break;

		case ShaderType::SHADER_GEOMETRY:
			Renderer_D3D::getDevice()->CreateGeometryShader(data, size, NULL, &mGeometryShader);
		break;
	}

}

void Shader::loadInputElemnts(void *data, size_t size)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> ieds;
	ID3D11ShaderReflection *shaderRefl = nullptr;
	HR(D3DReflect(data, size, IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&shaderRefl)));

	D3D11_SHADER_DESC shaderDesc;
	shaderRefl->GetDesc(&shaderDesc);
	for (int i = 0; i < shaderDesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		shaderRefl->GetInputParameterDesc(i, &paramDesc);

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;	
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		//save element desc
		ieds.push_back(elementDesc);
	}

	Renderer_D3D::getDevice()->CreateInputLayout(ieds.data(), ieds.size(), data, size, &mInputLayout);
	Renderer_D3D::getDevContext()->IASetInputLayout(mInputLayout);
	SafeRelease(shaderRefl);
}

void Shader::Bind(UINT type)
{
	if (type & ShaderType::SHADER_VERTEX)
	{
		Renderer_D3D::getDevContext()->VSSetShader(mVertShader, 0, 0);
		Renderer_D3D::getDevContext()->IASetInputLayout(mInputLayout);
	}
	if (type & ShaderType::SHADER_PIXEL)
		Renderer_D3D::getDevContext()->PSSetShader(mPixelShader, 0, 0);
	if (type & ShaderType::SHADER_GEOMETRY)
		Renderer_D3D::getDevContext()->GSSetShader(mGeometryShader, 0, 0);
}

void Shader::unBind(UINT type)
{
	if (type & ShaderType::SHADER_VERTEX)
	{
		Renderer_D3D::getDevContext()->VSSetShader(nullptr, 0, 0);
	}
	if (type & ShaderType::SHADER_PIXEL)
		Renderer_D3D::getDevContext()->PSSetShader(nullptr, 0, 0);
	if (type & ShaderType::SHADER_GEOMETRY)
		Renderer_D3D::getDevContext()->GSSetShader(nullptr, 0, 0);
}

Shader::~Shader()
{
	SafeRelease(mVertShader);
	SafeRelease(mPixelShader);
	SafeRelease(mInputLayout);
}
