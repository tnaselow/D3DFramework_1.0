#include "Ocean_H.hlsl"

float4 main(in PS_IN IN) : SV_TARGET
{
	float3 n = normalize(IN.m_WorldNormal);
	float3 l = -normalize(float3(1, 1, -1));

	float u = max(dot(n, l), 0);
	float3 v = -IN.m_WorldPosition;
	float3 halfway = normalize(l + v);

	float3 ambient = (0.0, 0.1, 0.2);
	float3 diffuse = float3(0.0, 0.4, 1.0) * u;
	float3 specular = float3(1.0, 0.25, 0.0) * max(pow(dot(halfway, n), 120), 0);

	//float3 color = float3(0.0, 0.4, 1.0) * u;

	return float4(specular + ambient + diffuse, 1.0);
}