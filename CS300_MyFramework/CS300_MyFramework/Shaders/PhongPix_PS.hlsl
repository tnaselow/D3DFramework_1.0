/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongPix_PS.hlsl
Purpose: To do the lighting calculation for phong-blinn lighting
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "PhongPix_H.hlsl"


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

Texture2D diffuseTexture : register(t0);
Texture2D normalMap : register(t1);

SamplerState textureSampler 
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

// BUFFERS
cbuffer LIGHT_BUFFER : register(b2)
{
	Light lights[8];

	int numLights;
	float3 globalAmbient;

	float innerRadius;
	float outerRadius;
	float falloff;
	float specularIntesity;

	float3 attCoeffs;
	int useBlinn;

	float3 fogColor;
	int useTexture;

	float3 camPosition;
	float allign_4;
}

cbuffer MATERIAL_BUFFER : register(b3)
{
	Material material;
}

cbuffer MISC_BUFFER : register(b6)
{
	int useNormalMapTex;
	float blendEffect;
}


float3 calcAmbient(Light light)
{
	return light.ambient * material.ambient;
}

float3 calcDiffuse(Light light, float3 N, float3 L, float2 uvs)
{
	if(useTexture)
		return light.diffuse * diffuseTexture.Sample(textureSampler, uvs) * max(dot(N, L), 0);
	else
		return light.diffuse * material.diffuse * max(dot(N, L), 0);
}

float3 calcSpecularBlinn(Light light, float3 N, float3 L, float3 V)
{
	//TODO: add system to switch blinn to phong

	float3 halfwayDir = normalize(L + V);
	return light.specular * material.specular * pow(saturate(dot(N, halfwayDir)), specularIntesity);
}

float3 calcSpecularPhong(Light light, float3 V, float3 R)
{
	return light.specular * material.specular * pow(max(dot(V, R), 0), specularIntesity);
}

float calcAttenuation(float dist)
{
	float t1 = attCoeffs.x;
	float t2 = attCoeffs.y * dist;
	float t3 = attCoeffs.z * dist * dist;
	return 1.0 / (t1 + t2 + t3);
}

float calcSpotlightFactor(Light light, float3 worldPos)
{
	// direction of spotlight
	float3 lightDir = normalize(-light.direction.xyz);
	// direction from light to pixel
	float3 dirToVert = normalize(light.position.xyz - worldPos);
	float lDotd = dot(lightDir, dirToVert);
	float spotLightFac = (lDotd - cos(outerRadius)) / (cos(innerRadius) - cos(outerRadius));
	//spotLightFac = spotLightFac > 0 ? spotLightFac : 0;
	spotLightFac = max(spotLightFac, 0.0);
	return pow(spotLightFac, falloff);
}


float4 main(PS_IN IN) : SV_TARGET
{

	float4x4 TBN;
	TBN[0] = float4(IN.tangent, 0);
	TBN[1] = float4(IN.biTangent, 0);
	TBN[2] = float4(IN.worldNormal, 0);
	TBN[3] = float4(0, 0, 0, 1);
	transpose(TBN);
	float3 N = normalMap.Sample(textureSampler, IN.uvCoords);
	if (useNormalMapTex)
		return float4(abs(N), 1);
	N = (N * 2) - float3(1,1,1);


	N = mul(float4(N, 0), TBN);
	N = normalize(N);

	float zFar = 100;
	float zNear = 50;

	//float3 N = IN.worldNormal;
	//N = normalize(N);

	float3 color = float3(0,0,0);
	
	for (int i = 0; i < numLights; ++i)
	{
		// calculate light vector
		float3 L;
		if (lights[i].type == 2)
			L = lights[i].position.xyz - IN.worldPosition;
		else
			L = -lights[i].direction.xyz;
		float lightDist = length(L);
		L = normalize(L);

		// calculate view vector and view distance
		float3 V =  -IN.worldPosition;
		float distV = length(V);
		V = normalize(V);

		// calculate reflection vector
		//float3 R = 2 * dot(N, L) * N - L;
		float3 R = reflect(-L, N);
		R = normalize(R);

		float3 Ia = calcAmbient(lights[i]);
		float3 Id = calcDiffuse(lights[i], N, L, IN.uvCoords);
		float3 Is;
		if(useBlinn)
			Is = calcSpecularBlinn(lights[i], N, L, V);
		else
			Is = calcSpecularPhong(lights[i], V, R);
		float att = calcAttenuation(lightDist);


		float3 tempColor = Id + Is;
		// account for spotlight light
		if(lights[i].type == 3)
			tempColor *= calcSpotlightFactor(lights[i], IN.worldPosition);

		// account for attenuation
		if (lights[i].type > 1)
		{
			tempColor *= att;
			tempColor += Ia * att;
		}

		// calculate fog
		float S = saturate((zFar - distV) / (zFar - zNear));
		tempColor = (1 - S) * fogColor + (S * tempColor);

		// compensate for dotp check
		// zeros out color if facing the wrong way
		float faceCheck = (dot(N, L) > 0 ? 1 : 0);

		color += tempColor;
	}

	//return normalMap.Sample(textureSampler, IN.uvCoords);
	//return diffuseTexture.Sample(textureSampler, IN.uvCoords);
	//V = normalize(V);
	//float3 halfwayDir = normalize(L + V);
	//float3 R = 2 * dot(N, L) * N - L;//reflect(-L, N);
	//R = normalize(R);
	
	//return float4(-V, 1);
	//return float4(max(dot(V, R), 0), 0, 0, 1);//, specularIntesity);
	
	//float3 L = normalize(-lights[0].direction.xyz);
	//float doto = 2 * dot(N, L);
	//float3 scaledN = doto * N;
	//
	//float3 R = scaledN - L;
	//
	//float3 V = camPosition.xyz - IN.worldPosition;
	//V = normalize(V);
	//
	//float val = pow(max(dot(V, R), 0), 32);
	//return float4(val, 0, 0, 1);
	return float4(color, 1);
}