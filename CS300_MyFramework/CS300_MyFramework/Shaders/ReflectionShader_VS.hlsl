#include "SkyBox_H.hlsl"

cbuffer PROJECTION_BUFFER : register(b0)
{
	float4x4 ViewSpace;
	float4x4 Projection;
};

cbuffer MODEL_BUFFER : register(b1)
{
	float4x4 Model;
	float4x4 NormalModel;
};

PS_IN main(in VS_IN IN) 
{
	PS_IN OUT;
	OUT.position = mul(Projection, mul(ViewSpace, mul(Model, float4(IN.position.xyz, 1))));
	OUT.normal = mul(Model, float4(IN.normal.xyz, 0));
	//OUT.normal = mul(ViewSpace, OUT.normal);
	OUT.normPosition = normalize(mul(ViewSpace, mul(Model, float4(IN.position.xyz, 1))));
	return OUT;
}