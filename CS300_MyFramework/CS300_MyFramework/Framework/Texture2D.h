#pragma once

#include <DX/D3D11.h>
#include <DX/D3DX11.h>
#include <DX/D3DX10.h>

struct Texture2D
{
	int m_Width, m_Height;
	std::string m_Name;
	ID3D11ShaderResourceView *m_SRV;
};
