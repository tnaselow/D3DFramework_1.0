#include "DeferredG_H.hlsl"

cbuffer PROJECTION_BUFFER : register(b0)
{
	float4x4 Projection;
};

cbuffer MODEL_BUFFER : register(b1)
{
	float4x4 Model;
	float4x4 NormalModel;
};

VOut main( float3 pos : POSITION, float3 normal : NORMAL, float2 texCoord : TEX_COORD ) 
{
	VOut OUT;
	OUT.worldPos = mul(Model, float4(pos,1)); 
	OUT.position = mul(Projection, float4(OUT.worldPos, 1)); 
	OUT.normal = mul(Model, float4(normal,0));
	OUT.normal = normalize(OUT.normal);
	OUT.texCoord = texCoord;
	return OUT;
}