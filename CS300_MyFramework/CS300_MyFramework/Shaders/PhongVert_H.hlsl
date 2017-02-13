#pragma once


struct VS_IN
{
	float4 position : POSITION;
	float4 normal : NORMAL;
};


struct PS_IN
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};