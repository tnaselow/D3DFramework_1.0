/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Color_PS.hlsl
Purpose: Do lighting and coloring on pixels
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "Color_H.hlsl"


struct Light
{
	float4 direction;
	float4 ambient;
	float4 diffuse;
};

struct Material
{
	float4 ambient;
	float4 diffuse;
};

cbuffer LIGHTS_BUFFER : register(b2)
{
	Light lights[8];
	int numLights;

	float3 padding;
};

cbuffer MATERIAL_BUFFER : register(b3)
{
	Material material;
};

float4 main(in VOut IN) : SV_TARGET
{
	// used as a flag from the geometry shader to determine if this is a line or triangle
	if (IN.color.a == 0)
		return float4(0,0,0,1);

	// sets color to black 
	float4 color = float4(0, 0, 0, 0);
	// loop over all the lights
	for (int i = 0; i < numLights; ++i)
	{
		Light light = lights[i];
		// get normal and light directino vectors
		float3 L = normalize(-light.direction).xyz;
		float3 N = normalize(IN.normal);
		// find ambient color of the surface
		float4 ambient = light.ambient * material.ambient;
		float4 diffuse = float4(0,0,0,0);
		// finds the diffuse factor : surface normal's relationship to the light
		float diffuseFactor = dot(N, L);

		if (diffuseFactor > 0) 
		{
			diffuse = diffuseFactor * light.diffuse * material.diffuse;
		}
		// adds ambient and diffuse to total color
		color += ambient + diffuse;
	}

	return color;
}
