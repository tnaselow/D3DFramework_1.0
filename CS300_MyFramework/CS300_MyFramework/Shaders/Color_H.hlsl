/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Color_H.hlsl
Purpose: Has common data structures for color shaders
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#pragma once

struct PixelOutput
{
	float4 Color : SV_TARGET0;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct ModelData
{
	float3 position;
	float3 rotation;
	float3 scale;
};