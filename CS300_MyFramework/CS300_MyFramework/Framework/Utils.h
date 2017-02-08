/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Utils.h
Purpose: To provide utility functions to the rest of the code
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#pragma once
#include <assert.h>
#ifdef _DEBUG
#define HR(x) assert(x == 0)
#else
#define HR(x) x
#endif

template<typename T>
void SafeRelease(T &COM)
{
	if(COM)
	{
		COM->Release();
		COM = nullptr;
	}
}