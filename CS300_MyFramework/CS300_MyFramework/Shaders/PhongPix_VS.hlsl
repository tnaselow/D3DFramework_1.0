/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongPix_VS.hlsl
Purpose: To send transform and send data to pixel shader for lighting
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "PhongPix_H.hlsl"

// BUFFERS
cbuffer PROJECTION_BUFFER : register(b0)
{
	float4x4 Projection;
};

cbuffer MODEL_BUFFER : register(b1)
{
	float4x4 Model;
	float4x4 NormalModel;
};

PS_IN main( VS_IN IN) 
{
	PS_IN OUT;
	float4 worldpos = mul(Model, float4(IN.position, 1));
	OUT.position = mul(Projection, worldpos);
	OUT.worldPosition = worldpos;
	OUT.worldNormal = mul(Model, float4(IN.normal, 0));
	OUT.uvCoords = IN.texCoords;
	
	OUT.tangent = mul(Model, float4(IN.tangent, 0));
	OUT.biTangent = mul(Model, float4(IN.biTangent, 0));

	return OUT;
}