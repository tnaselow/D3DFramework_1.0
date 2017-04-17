#include "SkyBox_H.hlsl"

SamplerState textureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer MISC_BUFFER : register(b6)
{
	int useNormalMapTex;
	float blendEffect;
}

TextureCube enviornTexture : register(t3);

Texture2D right  : register(t4);
Texture2D left   : register(t5);
Texture2D top    : register(t6);
Texture2D bottom : register(t7);
Texture2D back   : register(t8);
Texture2D front  : register(t9);


float4 sampleTexture(float3 lookup)
{
	float x = lookup.x;
	float y = lookup.y;
	float z = lookup.z;

	if (abs(z) > abs(y) && abs(z) > abs(x))
	{
		if (z > 0)
			return front.Sample(textureSampler, float2(x, y));
		else
			return back.Sample(textureSampler, float2(x, y));
	}
	else if (abs(x) > abs(z) && abs(x) > abs(y))
	{
		if (x > 0)
			return left.Sample(textureSampler, float2(x, y));
		else
			return right.Sample(textureSampler, float2(x, y));
	}
	else if (abs(y) > abs(z) && abs(y) > abs(x))
	{
		if (y > 0)
			return top.Sample(textureSampler, float2(x, y));
		else
			return bottom.Sample(textureSampler, float2(x, y));
	}
	return float4(0, 0, 0, 1);
}

float4 main(in PS_IN IN) : SV_TARGET
{
	float3 N = normalize(IN.normal);
	float3 V = IN.normPosition;
	float3 R = 2 * dot(N, V) * N - V;
	//float3 R = reflect(-V, N);
	R.y *= -1;

	float K = 1 / 1.3;
	float3 T = K * dot(N, -V) - sqrt(1 - K * K * (1 - dot(N, -V) * dot(N, -V))) * N - K * -V;
	//T = refract(-V, N, K);
	float4 refrColor = enviornTexture.Sample(textureSampler, T);
	float4 reflColor = enviornTexture.Sample(textureSampler, R);
	//float4 refrColor = sampleTexture(T);
	//float4 reflColor = sampleTexture(R);

	return (1 - blendEffect) * reflColor + blendEffect * refrColor;
}