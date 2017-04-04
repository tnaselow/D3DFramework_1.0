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
	float4x4 ViewSpace;
	float4x4 Projection;
};

cbuffer MODEL_BUFFER : register(b1)
{
	float4x4 Model;
	float4x4 NormalModel;
};

cbuffer COLOR_BUFFER : register(b5)
{
	float4 color;
}


VOut main(VS_IN IN)
{
	VOut output;

	// calculate the perspective projected position
	output.position = mul(Projection, mul(ViewSpace, mul(Model, float4(IN.position, 1))));
	output.color = color;

	return output;
}