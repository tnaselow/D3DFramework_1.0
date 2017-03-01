/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Renderer_D3D.h
Purpose: To wrap DX11 calls in an easier to use API
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#pragma once

#include <DX/D3D11.h>
#include <DX/D3DX11.h>
#include <DX/D3DX10.h>
#include <vector>
#include "Sandbox.h"
#include "Shader.h"

struct AssimpModel;
class Mesh;
class Shader;

enum BufferUsage
{
	USAGE_DYNAMIC,
	USAGE_CONSTANT
};

enum BufferTypes
{
	BUFFER_PROJECTION = 0,
	BUFFER_MODEL ,
	BUFFER_LIGHTS,
	BUFFER_MATERIAL,
	BUFFER_NORMAL_TYPE,
	BUFFER_COLOR,

	BUFFER_NUM_BUFFERS
};

struct EntityShader
{
	EntityShader(Entity entity, Shader *shader) : mEntity(entity), mShader(shader) {}
	Entity mEntity;
	Shader *mShader;
};

class Renderer_D3D
{
	public:
		static void Initialize(HWND hwnd, int width, int height);

		static void CleanUp();

		static void DrawEntity(Entity &entity, Shader &shader);
		static void DrawModel(AssimpModel &model, Shader &shader);
	
		static void EndFrame();
		static void EndFrameDeffered();

		static ID3D11Device	*getDevice() { return mDevice; }
		static ID3D11DeviceContext *getDevContext() { return mDeviceContext; }

		static void resizeBackBuffer(LPARAM lParam);

		static void setLightBuffer(const LightBufferData &lights);
		static void setNormalDrawtype(unsigned type);

		static void makeCBuffer(BufferTypes buffer, size_t size, BufferUsage usage, const void *data = nullptr);
		static void mapCBuffer(BufferTypes buffer, size_t size, const void *data, unsigned bindType, int bindRegister = -1);

		static ID3D11RasterizerState  *mRasterState;
	private:
		static std::vector<EntityShader> mEntities;

		static ID3D11Device		   *mDevice;
		static ID3D11DeviceContext *mDeviceContext;
		static IDXGISwapChain	   *mSwapChain;

		static ID3D11RenderTargetView *mRTV_BackBuffer;
		static ID3D11DepthStencilView *mRTV_DepthStencil;
		static ID3D11Texture2D		  *mBackBuffTexture;
		static ID3D11Texture2D		  *mDepthStencilTexture;
		static ID3D11Debug			  *mDebugInterface;

		static ID3D11ShaderResourceView *m_WrapTextureView;

		static std::vector<ID3D11Buffer *> mC_Buffers;

		Renderer_D3D();
};
