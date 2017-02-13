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
	int falloff;
	float allign_1;
	
	float3 attCoeffs;
	float allign_2;
	
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
	return mul(light.ambient, material.ambient);
}

float3 calcDiffuse(Light light, float3 N, float3 L)
{
	return mul(mul(light.diffuse, max(dot(N, L), 0.0)), material.diffuse);
}

float3 calcSpecular(Light light, float3 V, float3 R)
{
	return mul(mul(light.specular, pow(max(dot(V, R), 0.0f), falloff)), material.specular);
}

float calcAttenuation(float distance)
{
	float c1 = attCoeffs.x;
	float c2 = attCoeffs.y * distance;
	float c3 = attCoeffs.z * distance * distance;
	return min(1, 1.0 / (c1 + c2 + c3));
}

float zFar = 100;
float zNear = 50;

PS_IN main( VS_IN IN ) 
{
	PS_IN OUT;
	OUT.position = mul(Projection, mul(Model, IN.position));

	float3 N = normalize(IN.normal.xyz);

	float3 color = float3(0, 0, 0);
	for (int i = 0; i < numLights; ++i)
	{
		// calculate light vector and light distance
		float3 L;
		if (lights[i].type == 1)
			L = -lights[i].direction.xyz;
		else
			L = lights[i].position.xyz - IN.position.xyz;
		
		float distL = length(L);
		L = normalize(L);


		// calculate view vector and view distance
		float3 V = camPosition.xyz - IN.position.xyz;
		float distV = length(V);
		V = normalize(V);

		// calculate the reflection vector
		float3 R = mul((2 * dot(N, L)), N - L);
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

		// calculate fog
		float S = (zFar - distV) / (zFar - zNear);
		tempColor = ((1 - S) * tempColor) + (S * fogColor);

		color += tempColor;
	}

	OUT.color = float4(color, 1);
	return OUT;
}