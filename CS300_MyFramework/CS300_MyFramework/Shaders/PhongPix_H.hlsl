#define PI 3.14159265359

struct VS_IN
{
	float4 position  : POSITION;
	float4 normal    : NORMAL;
	float4 texCoords : UV_COORDS; // only stored in xy
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float3 worldNormal : NORMAL;
	float3 worldPosition : POSITION;
	float2 uvCoords : UV_COORDS;
};