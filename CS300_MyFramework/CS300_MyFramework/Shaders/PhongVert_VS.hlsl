/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Renderer_D3D.cpp
Purpose: To do phong lighting in vert shader
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "PhongVert_H.hlsl"

// DEFINES
struct Light
{
	float4 position;
	float4 direction;
		 
	float4 ambient;
	float4 diffuse;
	float4 specular;

	int type;

	float3 allign;
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

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

cbuffer LIGHT_BUFFER : register(b2)
{
	Light lights[8];

	int numLights;
	float3 globalAmbient;

	float innerRadius;
	float outerRadius;
	float falloff;
	float allign_1;
	
	float3 attCoeffs;
	int useBlinn;
	
	float3 fogColor;
	float allign_3;

	float3 camPosition;
	float allign_4;
}

cbuffer MATERIAL_BUFFER : register(b3)
{
	Material material;
}

float3 calcAmbient(Light light)
{
	return light.ambient * material.ambient;
}

float3 calcDiffuse(Light light, float3 N, float3 L)
{
	return light.diffuse * max(dot(N, L), 0.0) * material.diffuse;
}

float3 calcSpecular(Light light, float3 V, float3 R)
{
	// specular power set to 1 for lack of texture
	return light.specular * pow(max(dot(V, R), 0.0f), 1.0) * material.specular;
}

float calcSpotlightFactor(Light light, float3 worldPos)
{
	// direction of spotlight
	float3 lightDir = normalize(-light.direction.xyz);
	// direction from light to pixel
	float3 dirToVert = normalize(light.position.xyz - worldPos);
	float lDotd = dot(lightDir, dirToVert);
	return pow((lDotd - cos(outerRadius)) / (cos(innerRadius) - cos(outerRadius)), falloff);
}

float calcAttenuation(float distance)
{
	float c1 = attCoeffs.x;
	float c2 = attCoeffs.y * distance;
	float c3 = attCoeffs.z * distance * distance;
	return min(1, 1.0 / (c1 + c2 + c3));
}
PS_IN main( VS_IN IN ) 
{
	PS_IN OUT;
	OUT.position = mul(Projection, mul(Model, IN.position));

	float zFar = 100;
	float zNear = 50;

	float3 worldPos = mul(Model, IN.position).xyz;
	float3 worldNorm = mul(Model, IN.normal).xyz;

	float3 N = normalize(worldNorm);

	float3 color = float3(0, 0, 0);
	for (int i = 0; i < numLights; ++i)
	{
		float3 L;
		// calculate light vector and light distance
		if (lights[i].type == 1)
			L = -lights[i].direction.xyz;
		else
			L = lights[i].position.xyz - worldPos;
		
		float distL = length(L);
		L = normalize(L);


		// calculate view vector and view distance
		float3 V = camPosition.xyz - worldPos;
		float distV = length(V);
		V = normalize(V);
		
		// calculate the reflection vector
		float3 R = (2 * dot(N, L) * N) - L;
		R = normalize(R);

		// calculate phong lighting
		float3 Ia = calcAmbient(lights[i]);
		float3 Id = calcDiffuse(lights[i], N, L);
		float3 Is = calcSpecular(lights[i], V, R);
		float att = calcAttenuation(distL);

		float3 tempColor = Ia + Id + Is;
		// if it is not a directional light use attenuation
		if (lights[i].type > 1)
			tempColor *= att;

		float spotlightFactor = calcSpotlightFactor(lights[i], worldPos);
		if (lights[i].type == 3)
			tempColor *= spotlightFactor;

		// calculate fog
		float S = saturate( (zFar - distV) / (zFar - zNear) );
		tempColor = ((1 - S) * fogColor) + (S * tempColor);

		// compensate for dotp check
		// zeros out color if facing the wrong way
		float faceCheck = (dot(N, L) > 0 ? 1 : 0);

		color += tempColor * faceCheck;
	}

	
	OUT.color = float4(color, 1);
	return OUT;
}
