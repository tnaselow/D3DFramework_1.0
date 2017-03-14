#pragma once

struct VS_IN
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;

	float2 texCoords : TEX_COORDS;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float3 worldNormal : NORMAL;
	float3 worldPosition : POSITION;
	float2 uvCoords : UV_COORDS;
};