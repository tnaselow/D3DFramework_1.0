/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Texture2D.h
Purpose: To provide a structure for texture data
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

struct Texture2D
{
	int m_Width, m_Height;
	std::string m_Name;
	ID3D11ShaderResourceView *m_SRV = nullptr;
	ID3D11RenderTargetView   *m_RTV = nullptr;
	ID3D11DepthStencilView   *m_DSV = nullptr;
};
