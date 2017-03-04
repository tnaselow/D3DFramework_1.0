#include "DeferredG_H.hlsl"

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);

SamplerState textureSampler : register(s0);

PS_OUT main(VOut IN) 
{
	PS_OUT OUT;

	OUT.color = diffuseTexture.Sample(textureSampler, IN.texCoord);
	OUT.specular = specularTexture.Sample(textureSampler, IN.texCoord);
	OUT.normal = float4(IN.normal, 0);
	OUT.position = float4(IN.worldPos, 1);
	
	return OUT;
}