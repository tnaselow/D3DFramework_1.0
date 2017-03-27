#include "Ocean_H.hlsl"

cbuffer PROJECTION_BUFFER : register(b0)
{
	float4x4 Projection;
};

cbuffer MODEL_BUFFER : register(b1)
{
	float4x4 Model;
	float4x4 NormalModel;
};

PS_IN main( in VS_IN IN ) 
{
	PS_IN OUT;

	OUT.m_OutPos = mul(Projection, mul(Model, float4(IN.m_NewPos, 1)));
	OUT.m_WorldNormal = mul(Projection, mul(Model, float4(IN.m_Normal, 1)));
	return OUT;
}