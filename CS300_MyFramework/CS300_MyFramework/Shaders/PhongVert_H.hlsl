/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongVert_H.hlsl
Purpose: To provide data structures for phong lighting in the vs
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#pragma once


struct VS_IN
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 texCoords : UV_COORDS;
};


struct PS_IN
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};