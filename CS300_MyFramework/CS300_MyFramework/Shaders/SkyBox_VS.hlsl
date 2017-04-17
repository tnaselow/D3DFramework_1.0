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
	float4x4 camMat = ViewSpace;
	camMat[0][3] = 0;
	camMat[1][3] = 0;
	camMat[2][3] = 0;

	OUT.position = mul(Projection, mul(camMat, mul(Model, float4(IN.position.xyz, 1))));
	// used to write to far end of depth buffer // ask about
	OUT.position = OUT.position.xyww;

	OUT.texCoords = normalize(IN.position.xyz);
	OUT.normPosition = normalize(mul(Model, float4(IN.position.xyz, 1)));

	return OUT;
}