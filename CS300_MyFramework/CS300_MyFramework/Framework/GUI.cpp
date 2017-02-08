/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GUI.cpp
Purpose: small wrapper around certain imgui functions
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "GUI.h"
#include "Utils.h"


void GUI::Initialize(HWND hwnd, ID3D11Device *device, ID3D11DeviceContext *devContext)
{
	ImGui_ImplDX11_Init(hwnd, device, devContext);
}

void GUI::UpdateAndDraw()
{
	//ImGui_ImplDX11_NewFrame();
	// do imgui rendering
	//ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
	//ImGui::Begin("Another Window");
	//ImGui::Text("Hello");
	//ImGui::End();

	ImGui::Render();
}

void GUI::CleanUp()
{
	ImGui_ImplDX11_Shutdown();
}

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool GUI::Input(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplDX11_WndProcHandler(hwnd, message, wParam, lParam))
		return true;
	else
		return false;
}
