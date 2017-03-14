#include "Normal_H.hlsl"

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

PS_IN main( VS_IN IN) 
{
	PS_IN OUT;

	OUT.worldPosition = mul(Model, float4(IN.position, 1));
	OUT.position = mul(Projection, float4(OUT.worldPosition, 1));
	OUT.worldNormal = mul(Model, float4(IN.normal, 0));
	OUT.uvCoords = IN.texCoords;

	return OUT;
}