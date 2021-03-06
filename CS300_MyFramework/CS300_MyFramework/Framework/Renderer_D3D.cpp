/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Renderer_D3D.cpp
Purpose: To wrap DX11 calls in an easier to use API
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "Renderer_D3D.h"
#include "Utils.h"
#include "GUI.h"
#include <vector>
#include "Mesh.h"
#include "Sandbox.h"
#include "glm/gtx/transform.hpp"
#include <iostream>
#include "ResourceManager.h"
#include "Camera.h"

ID3D11Device		*Renderer_D3D::mDevice;
ID3D11DeviceContext *Renderer_D3D::mDeviceContext;
IDXGISwapChain		*Renderer_D3D::mSwapChain;

ID3D11RenderTargetView *Renderer_D3D::mRTV_BackBuffer;
ID3D11DepthStencilView *Renderer_D3D::mRTV_DepthStencil;
ID3D11Texture2D		  *Renderer_D3D::mBackBuffTexture;
ID3D11Texture2D		  *Renderer_D3D::mDepthStencilTexture;
ID3D11Debug			  *Renderer_D3D::mDebugInterface;
ID3D11RasterizerState *Renderer_D3D::mRasterState;
std::vector<ID3D11Buffer *> Renderer_D3D::mC_Buffers(BUFFER_NUM_BUFFERS);

std::vector<EntityShader>    Renderer_D3D::mEntities;
ID3D11Buffer           *Renderer_D3D::mLineBuffer;
ID3D11DepthStencilState *Renderer_D3D::mDepthStencilState;

Texture2D *Renderer_D3D::mEnviornTextures[6];




bool Renderer_D3D::m_RenderNormals    = false;
bool Renderer_D3D::m_RenderTangents   = false;
bool Renderer_D3D::m_RenderBiTangents = false;
int Renderer_D3D::m_UseNormalMapAsTex = false;
float Renderer_D3D::m_RRBlendFactor = 0;




void Renderer_D3D::Initialize(HWND hwnd, int width, int height)
{
	// Mandatory DX11 usage 
	D3D_FEATURE_LEVEL featureLevels[1] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL supportedLevel;
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));

	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;
	swapDesc.OutputWindow = hwnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	HR(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, featureLevels, sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION, &swapDesc, &mSwapChain, &mDevice, &supportedLevel, &mDeviceContext));

	HR(mDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&mDebugInterface)));

	// create back buffer
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&mBackBuffTexture));
	HR(mDevice->CreateRenderTargetView(mBackBuffTexture, NULL, &mRTV_BackBuffer));

	SafeRelease(mBackBuffTexture);
	
	// add depth and stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(mDevice->CreateTexture2D(&depthStencilDesc, NULL, &mDepthStencilTexture));
	HR(mDevice->CreateDepthStencilView(mDepthStencilTexture, NULL, &mRTV_DepthStencil));
	
	mDeviceContext->OMSetRenderTargets(1, &mRTV_BackBuffer, mRTV_DepthStencil);

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	//rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	
	HR(mDevice->CreateRasterizerState(&rasterDesc, &mRasterState));

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	mDeviceContext->RSSetViewports(1, &vp);
	

	// init GUI
	GUI::Initialize(hwnd, mDevice, mDeviceContext);

	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof(Vertex) * 20000;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	HR(mDevice->CreateBuffer(&buffDesc, nullptr, &mLineBuffer));


	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = true;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(mDevice->CreateDepthStencilState(&dsDesc, &mDepthStencilState));
	mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 0xFF);

	// init projection buffer
	//glm::mat4x4 projMat = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	//glm::mat4 camMat = cam.getTransform();
	//glm::mat4 proj[2] =
	//{
	//	camMat,
	//	projMat
	//};

	// generate enviornment map texture stuff -------------------------
	for(int i = 0; i < 6; ++i)
		mEnviornTextures[i] = ResourceManager::createTexture("Enviorn" + i, true, true, 2048, 2048, nullptr, false);

	depthStencilDesc.Width = 2048;
	depthStencilDesc.Height = 2048;

	ID3D11Texture2D *edTexture;
	HR(mDevice->CreateTexture2D(&depthStencilDesc, NULL, &edTexture));
	HR(mDevice->CreateDepthStencilView(edTexture, NULL, &mEnviornTextures[0]->m_DSV));
	SafeRelease(edTexture);


	//-------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------

	makeCBuffer(BUFFER_PROJECTION, sizeof(glm::mat4) * 2, USAGE_DYNAMIC);
	//makeCBuffer(BUFFER_PROJECTION, sizeof(glm::mat4) * 2, USAGE_DYNAMIC, &projMat);
	makeCBuffer(BUFFER_MODEL, sizeof(glm::mat4x4) * 2, USAGE_DYNAMIC);
	makeCBuffer(BUFFER_LIGHTS, sizeof(LightBufferData), USAGE_DYNAMIC);
	makeCBuffer(BUFFER_MATERIAL, sizeof(Material), USAGE_DYNAMIC);
	makeCBuffer(BUFFER_NORMAL_TYPE, 16, USAGE_DYNAMIC);
	makeCBuffer(BUFFER_COLOR, sizeof(glm::vec4), USAGE_DYNAMIC);
	makeCBuffer(BUFFER_MISC, 16, USAGE_DYNAMIC);
}

void Renderer_D3D::genEnviornMap(glm::vec3 pos, Camera cam)
{
	unsigned n = 1;
	D3D11_VIEWPORT vp;
	mDeviceContext->RSGetViewports(&n, &vp);
	int width = vp.Width;
	int height = vp.Height;

	vp.Width = 2048;
	vp.Height = 2048;
	mDeviceContext->RSSetViewports(1, &vp);


	glm::mat4x4 projMat = glm::perspectiveRH(90.0f, 1.0f, 0.1f, 100.0f);
	cam.setPosition(pos);
	float origYaw = cam.getYaw();
	for(int i = 0; i < 4; ++i)
	{
		// clear render target
		mDeviceContext->ClearRenderTargetView(mEnviornTextures[i]->m_RTV, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));
		mDeviceContext->ClearDepthStencilView(mEnviornTextures[0]->m_DSV, D3D11_CLEAR_DEPTH, 1, 0);

		// rotate camera
		cam.setYaw(origYaw + 90 * i);
		glm::mat4 camMat = cam.getTransform();
		glm::mat4 projData[2] =
		{
			camMat,
			projMat
		};
		Renderer_D3D::mapCBuffer(BUFFER_PROJECTION, sizeof(glm::mat4) * 2, projData, SHADER_VERTEX);

		mDeviceContext->OMSetRenderTargets(1, &mEnviornTextures[i]->m_RTV, mEnviornTextures[0]->m_DSV);
		
		// draw entities to frame buffer
		DrawEntities();
	}

	for(int i = 0; i < 6; ++i)
		mDeviceContext->PSSetShaderResources(i + 4, 1, &mEnviornTextures[i]->m_SRV);
	
	vp.Width = width;
	vp.Height = height;
	mDeviceContext->RSSetViewports(1, &vp);
	mDeviceContext->OMSetRenderTargets(1, &mRTV_BackBuffer, mRTV_DepthStencil);
}

void Renderer_D3D::makeCBuffer(BufferTypes buffer, size_t size, BufferUsage usage, const void *data)
{
	// must be 16 bit alligned
	assert(size % 16 == 0);
	// if it is constant data bust be provided
	if (data == nullptr)
		assert(usage != USAGE_CONSTANT);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	switch (usage)
	{
		case USAGE_CONSTANT:
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.CPUAccessFlags = 0;
			break;
		case USAGE_DYNAMIC:
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
	}

	
	HR(mDevice->CreateBuffer(&bd, nullptr, &mC_Buffers[buffer]));
}

void Renderer_D3D::mapCBuffer(BufferTypes buffer, size_t size, const void *data, unsigned bindType, int bindRegister)
{
	if (bindRegister == -1)
		bindRegister = buffer;
	assert(bindRegister < 15);
	assert(bindRegister < 15);

	if(data != nullptr)
	{
		D3D11_MAPPED_SUBRESOURCE map;
		ZeroMemory(&map, sizeof(map));
		HR(mDeviceContext->Map(mC_Buffers[buffer], 0, D3D11_MAP_WRITE_DISCARD, 0, &map));
		memcpy(map.pData, data, size);
		mDeviceContext->Unmap(mC_Buffers[buffer], 0);
	}

	if(bindType & SHADER_PIXEL)
		mDeviceContext->PSSetConstantBuffers(bindRegister, 1, &mC_Buffers[buffer]);
	if (bindType & SHADER_VERTEX)
		mDeviceContext->VSSetConstantBuffers(bindRegister, 1, &mC_Buffers[buffer]);
	if (bindType & SHADER_GEOMETRY)
		mDeviceContext->GSSetConstantBuffers(bindRegister, 1, &mC_Buffers[buffer]);
}

void Renderer_D3D::setNormalDrawtype(unsigned type)
{
	mapCBuffer(BUFFER_NORMAL_TYPE, sizeof(unsigned), &type, SHADER_GEOMETRY);
}

void Renderer_D3D::DrawEntity(Entity &entity, Shader &shader)
{
	mEntities.emplace_back(entity, &shader);
}

void Renderer_D3D::setLightBuffer(const LightBufferData &lights)
{
	mapCBuffer(BUFFER_LIGHTS, sizeof(lights), &lights, SHADER_PIXEL);
}

void Renderer_D3D::renderTangentsBiTangents(const Mesh &mesh, Lines line)
{
	std::vector<Vertex> lines;
	for(unsigned i = 0;  i < mesh.mVertices.size(); ++i)
	{
		Vertex v1;
		v1.Position = mesh.mVertices[i].Position;
		lines.push_back(v1);
		Vertex v2;
		glm::vec3 add;
		if (line == LINE_NORMAL)
			add = mesh.mVertices[i].Normal;
		else if (line == LINE_TANGENT)
			add = mesh.mVertices[i].Tangent;
		else if (line == LINE_BITANGENT)
			add = mesh.mVertices[i].BiTangent;

		v2.Position = mesh.mVertices[i].Position + add * 0.4f;
		lines.push_back(v2);
	}

	int size = lines.size() * sizeof(Vertex);
	D3D11_MAPPED_SUBRESOURCE map;
	ZeroMemory(&map, sizeof(map));
	HR(mDeviceContext->Map(mLineBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map));
	memcpy(map.pData, lines.data(), lines.size() * sizeof(Vertex));
	mDeviceContext->Unmap(mLineBuffer, 0);

	unsigned offset = 0;
	unsigned stride = sizeof(Vertex);

	mDeviceContext->IASetVertexBuffers(0, 1, &mLineBuffer, &stride, &offset);

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	mDeviceContext->Draw(lines.size(), 0);

}

void Renderer_D3D::DrawEntities()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	
	for(unsigned i = 0; i < mEntities.size(); ++i)
	{

		glm::mat4x4 model = glm::translate(mEntities[i].mEntity.mPosition) *
							glm::rotate(mEntities[i].mEntity.mRotation.z, glm::vec3(0, 0, 1)) *
							glm::rotate(mEntities[i].mEntity.mRotation.y, glm::vec3(0, 1, 0)) *
							glm::rotate(mEntities[i].mEntity.mRotation.x, glm::vec3(1, 0, 0)) * 
							glm::scale(mEntities[i].mEntity.mScale);
	
		glm::mat4x4 normModel = model;
		normModel  = glm::transpose(glm::inverse(normModel));
		glm::mat4x4 models[] = { model, normModel };

		mapCBuffer(BUFFER_MODEL, sizeof(glm::mat4x4) * 2, models, SHADER_VERTEX);
	
		// Draw tangents :)

		ResourceManager::getShader("Color")->Bind(SHADER_VERTEX | SHADER_PIXEL);
		glm::vec4 color;
		if(m_RenderNormals)
		{
			color = glm::vec4(1, 0, 0, 1);
			mapCBuffer(BUFFER_COLOR, sizeof(glm::vec4), &color, SHADER_VERTEX);
			renderTangentsBiTangents(*mEntities[i].mEntity.mMesh, LINE_NORMAL);
		}
		if(m_RenderTangents)
		{
			color = glm::vec4(0, 1, 0, 1);
			mapCBuffer(BUFFER_COLOR, sizeof(glm::vec4), &color, SHADER_VERTEX);
			renderTangentsBiTangents(*mEntities[i].mEntity.mMesh, LINE_TANGENT);
		}
		if(m_RenderBiTangents)
		{
			color = glm::vec4(0, 0, 1, 1);
			mapCBuffer(BUFFER_COLOR, sizeof(glm::vec4), &color, SHADER_VERTEX);
			renderTangentsBiTangents(*mEntities[i].mEntity.mMesh, LINE_BITANGENT);
		}

		mapCBuffer(BUFFER_MATERIAL, sizeof(Material), &mEntities[i].mEntity.mMaterial, SHADER_PIXEL);
		mapCBuffer(BUFFER_COLOR, sizeof(glm::vec4), &mEntities[i].mEntity.mColor, SHADER_VERTEX);

		struct miscBuffer
		{
			int usenormal;
			float blend;
		};

		miscBuffer ms;
		ms.usenormal = m_UseNormalMapAsTex;
		ms.blend = m_RRBlendFactor;
		mapCBuffer(BUFFER_MISC, sizeof(int) * 2, &ms, SHADER_PIXEL);

		mEntities[i].mShader->Bind(SHADER_VERTEX | SHADER_PIXEL);


		//mDeviceContext->VSSetConstantBuffers(1, 1, &mModelBuffer);

		mDeviceContext->IASetVertexBuffers(0, 1, &(mEntities[i].mEntity.mMesh->mVertBuffer), &stride, &offset);
		mDeviceContext->IASetIndexBuffer(mEntities[i].mEntity.mMesh->mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mDeviceContext->DrawIndexed(mEntities[i].mEntity.mMesh->mIndices.size(), 0, 0);
	}
}

void Renderer_D3D::EndFrame()
{
	mEntities.clear();
	Shader::unBind();

	GUI::UpdateAndDraw();
	mSwapChain->Present(0, 0);

	mDeviceContext->ClearRenderTargetView(mRTV_BackBuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));
	mDeviceContext->ClearDepthStencilView(mRTV_DepthStencil, D3D11_CLEAR_DEPTH, 1, 0);
}



void Renderer_D3D::CleanUp()
{
	GUI::CleanUp();
	for (int i = 0; i < BUFFER_NUM_BUFFERS; ++i)
		SafeRelease(mC_Buffers[i]);
	SafeRelease(mRTV_BackBuffer);
	SafeRelease(mBackBuffTexture);
	SafeRelease(mDepthStencilTexture);
	SafeRelease(mDebugInterface);
	SafeRelease(mSwapChain);
	SafeRelease(mDeviceContext);
	SafeRelease(mDevice);
}

void Renderer_D3D::resizeBackBuffer(LPARAM lParam)
{
	if (!mDevice)
		return;

	mDeviceContext->OMSetRenderTargets(0, NULL, NULL);
	//mDeviceContext->ClearState();

	unsigned width = (UINT)LOWORD(lParam);
	unsigned height = (UINT)HIWORD(lParam);

	SafeRelease(mRTV_BackBuffer);
	SafeRelease(mRTV_DepthStencil);


	glm::mat4x4 projMat = glm::perspective(45.0f, static_cast<float>(width)/ height, 0.1f, 100.0f);
	//D3D11_MAPPED_SUBRESOURCE map;
	//ZeroMemory(&map, sizeof(map));
	//HR(mDeviceContext->Map(mProjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map));
	//memcpy(map.pData, &projMat, sizeof(projMat));
	//mDeviceContext->Unmap(mProjBuffer, 0);
	//getDevContext()->VSSetConstantBuffers(0, 1, &Renderer_D3D::mProjBuffer);
	//getDevContext()->GSSetConstantBuffers(0, 1, &Renderer_D3D::mProjBuffer);
	Renderer_D3D::mapCBuffer(BUFFER_PROJECTION, sizeof(projMat), &projMat, SHADER_VERTEX | SHADER_GEOMETRY);

	//mDebugInterface->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	mSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

	// Create the render target
	ID3D11Texture2D* pBackBuffer;
	
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));

	HR(mDevice->CreateRenderTargetView(pBackBuffer, NULL, &mRTV_BackBuffer));
	// add depth and stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(mDevice->CreateTexture2D(&depthStencilDesc, NULL, &mDepthStencilTexture));
	HR(mDevice->CreateDepthStencilView(mDepthStencilTexture, NULL, &mRTV_DepthStencil));


	// temp no depth buff
	mDeviceContext->OMSetRenderTargets(1, &mRTV_BackBuffer, mRTV_DepthStencil);
	SafeRelease(pBackBuffer);

	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mDeviceContext->RSSetViewports(1, &vp);
}
