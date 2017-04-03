#pragma once

struct VS_IN
{
	float3 m_NewPos : NEW_POSITION;
	float3 m_Normal : NORMAL;
	float3 m_HTile0 : HTILDE0;
	float3 m_HTile0Conj : HTILDE0CONJ;
	float3 m_OrigPos : ORIG_POSITION;
};

struct PS_IN
{
	float4 m_OutPos : SV_POSITION;
	float3 m_WorldNormal : NORMAL;
	float3 m_WorldPosition : POSITION;
};