#include "SkyBox_H.hlsl"

SamplerState textureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

TextureCube skyBoxTexture : register(t3);

float4 main(in PS_IN IN) : SV_TARGET
{
	float3 N = normalize(IN.normal);
	float3 V = IN.normPosition;
	float3 R = 2 * dot(N, V) * N - V;
	//float3 R = reflect(-V, N);
	R.y *= -1;

	return skyBoxTexture.Sample(textureSampler, R);
}