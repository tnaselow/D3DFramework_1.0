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
	return light.diffuse * material.diffuse * max(dot(N, L), 0);
}

float3 calcSpecularBlinn(Light light, float3 N, float3 L, float3 V)
{
	//TODO: add system to switch blinn to phon
	float3 halfwayDir = normalize(L + V);
	return light.specular * material.specular * pow(max(dot(N, halfwayDir), 0.0), specularIntesity);
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
	float zFar = 100;
	float zNear = 50;

	float3 N = IN.worldNormal;
	N = normalize(N);

	float3 color = float3(0,0,0);
	
	for (int i = 0; i < numLights; ++i)
	{
		// calculate light vector
		float3 L;
		if (lights[i].type > 1)
			L = lights[i].position.xyz - IN.worldPosition;
		else
			L = -lights[i].direction.xyz;
		float lightDist = length(L);
		L = normalize(L);

		// calculate view vector and view distance
		float3 V = camPosition.xyz - IN.worldPosition;
		float distV = length(V);
		V = normalize(V);

		// calculate reflection vector
		float3 R = 2 * dot(N, L) * N - L;
		R = normalize(R);

		float3 Ia = calcAmbient(lights[i]);
		float3 Id = calcDiffuse(lights[i], N, L);
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


	return float4(color, 1);
}