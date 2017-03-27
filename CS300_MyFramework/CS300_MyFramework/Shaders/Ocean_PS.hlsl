#include "Ocean_H.hlsl"

float4 main(in PS_IN IN) : SV_TARGET
{
	float3 n = normalize(IN.m_WorldNormal);
	float3 l = -normalize(float3(-1, -1, -0.2));
	float u = max(dot(n, l), 0);


	return float4(0.0f, 0.0f, 1.0f, 1.0f) * u;
}