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
	return skyBoxTexture.Sample(textureSampler, IN.normPosition);
}