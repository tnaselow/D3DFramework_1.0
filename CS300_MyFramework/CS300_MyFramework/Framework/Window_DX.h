/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Window_DX.h
Purpose: To wrap a win32 window
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#pragma once
#include <windows.h>
#include <string>


class Window_DX
{
	public:
		Window_DX(const std::string &title = std::string("DX Window"), int width = 800, int height = 600);

		bool Loop();

		static int getWidth() { return mWidth; }
		static int getHeight() { return mHeight; }
	private:
		static LRESULT CALLBACK DefaultWinCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	private:
		HWND mWindHandle;
		static int mWidth, mHeight;
		HINSTANCE mHInstance;
		static bool mClosing;
};