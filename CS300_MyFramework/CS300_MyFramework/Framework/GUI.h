/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GUI.h
Purpose: small wrapper around certain imgui functions
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#pragma once

#include <windows.h>
#include <DX/D3D11.h>
#include <DX/D3DX11.h>
#include <DX/D3DX10.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"

class GUI
{
	public:
		static void Initialize(HWND hwnd, ID3D11Device *device, ID3D11DeviceContext *devContext);

		static void UpdateAndDraw();

		static void CleanUp();

		static bool Input(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		GUI(){}
};
