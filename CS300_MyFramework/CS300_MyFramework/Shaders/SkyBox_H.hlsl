
//struct VS_IN
//{
//	float4 position : POSITION;
//	float4 normal : NORMAL;
//	float4 texCoords : UV_COORDS;
//};



struct VS_IN
{
	//float4 position  : POSITION;
	//float4 normal    : NORMAL;
	//float4 texCoords : UV_COORDS; // only stored in xy not used in this assignment

	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoords : UV_COORDS;

	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;

};


struct PS_IN
{
	float4 position : SV_POSITION;
	float3 normPosition : NORM_POSITION;
	float3 texCoords : TEX_COORDS;
	float3 normal : NORMAL;
};