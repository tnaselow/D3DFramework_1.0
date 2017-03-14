/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongPix_H.hlsl
Purpose: To provide structures for the phong shader pipeline
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#define PI 3.14159265359

struct VS_IN
{
	//float4 position  : POSITION;
	//float4 normal    : NORMAL;
	//float4 texCoords : UV_COORDS; // only stored in xy not used in this assignment

	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoords : UV_COORDS;
	
	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;

};

struct PS_IN
{
	float4 position : SV_POSITION;
	float3 worldNormal : NORMAL;
	float3 worldPosition : POSITION;
	float2 uvCoords : UV_COORDS;

	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
};