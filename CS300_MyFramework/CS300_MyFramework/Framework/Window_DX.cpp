/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Window_DX.cpp
Purpose: To wrap a win32 window 
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "Window_DX.h"
#include "Utils.h"
#include "Renderer_D3D.h"
#include "GUI.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include <vector>
#include "Shader.h"
#include "Mesh.h"
#include "Sandbox.h"

bool Window_DX::mClosing = false;

LRESULT CALLBACK Window_DX::DefaultWinCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GUI::Input(hwnd, message, wParam, lParam))
		return 0;

	switch (message)
	{
	case WM_SIZE:
		if(wParam != SIZE_MINIMIZED)
		{
			if(Renderer_D3D::getDevice())
			{
				ImGui_ImplDX11_InvalidateDeviceObjects();
				Renderer_D3D::resizeBackBuffer(lParam);
				ImGui_ImplDX11_CreateDeviceObjects();
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		mClosing = true;
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}



Window_DX::Window_DX(const std::string &title, int width, int height) : mWidth(width), mHeight(height)
{
	WNDCLASSEX wcex;

	mHInstance = static_cast<HINSTANCE>(GetModuleHandleW(nullptr));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = DefaultWinCallback;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mHInstance;
	wcex.hIcon = LoadIcon(mHInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"CS300_Framework";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	//assert(> 0);
	RegisterClassEx(&wcex);
	std::wstring wideTitle(title.begin(), title.end());

	//TODO: fix width / height screen adjustment
//	RECT wr = { 0, 0, width, height };					  // set the size, but not the position
//	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size
//	width = wr.right - wr.left;
//	height = wr.bottom - wr.top;

	mWindHandle = CreateWindow( L"CS300_Framework", wideTitle.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, mHInstance, NULL );

	assert(mWindHandle > 0);

	Renderer_D3D::Initialize(mWindHandle, width, height);
	
	ShowWindow(mWindHandle, SW_SHOW);
	UpdateWindow(mWindHandle);

	Sandbox::initialize();

}

bool Window_DX::Loop()
{

	MSG msg;
	while (PeekMessage(&msg, mWindHandle, 0, 0, true))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Sandbox::update();
	return mClosing;
}