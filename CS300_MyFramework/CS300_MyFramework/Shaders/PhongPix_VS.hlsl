#include "PhongPix_H.hlsl"

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
	float4 worldpos = mul(Model, IN.position);
	OUT.position = mul(Projection, worldpos);
	OUT.worldPosition = worldpos;
	OUT.worldNormal = mul(Model, IN.normal).xyz;
	float r = length(IN.position);
	float temp = (atan(IN.position.z / IN.position.x) + (3.141596265358 / 2)) / (3.141596265358);
	float temp2 = (atan(IN.position.y / r) + (3.141596265358 / 2)) / (3.141596265358);
	OUT.uvCoords = float2(temp, temp2);

	return OUT;
}