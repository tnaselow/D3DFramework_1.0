
struct VOut
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEX_COORD;
};

struct PS_OUT
{
	float4 color : SV_TARGET0;
	float4 position : SV_TARGET1;
	float4 normal : SV_TARGET2;
	float4 specular : SV_TARGET3;
};