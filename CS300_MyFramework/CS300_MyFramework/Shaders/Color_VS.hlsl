/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Color_VS.hlsl
Purpose: Do Transforming of vertices
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "Color_H.hlsl"

cbuffer PROJECTION_BUFFER : register(b0)
{
	float4x4 Projection;
};

cbuffer MODEL_BUFFER : register(b1)
{
	float4x4 Model;
	float4x4 NormalModel;
};


VOut main(float4 position : POSITION, float4 normal : NORMAL)
{
	VOut output;

	// calculate the perspective projected position
	output.position = mul(Projection, mul(Model, position));
	// calculate the world position of this vertex
	output.pixelPos = mul(Model, position);
	// set a color (depricated not used any more)
	output.color = float4(1, 0, 0, 1);
	// sets a world space transformed normal of this veretex
	output.normal = mul(Model, normal).xyz;
	
	return output;
}