/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Color_G.hlsl
Purpose: Generate normal geometry for meshes
Language: C++
Platform: Microsoft Visual studio 14 Compiler
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/
#include "Color_H.hlsl"

cbuffer PROJECTION_BUFFER : register(b0)
{
	float4x4 Projection;
}

cbuffer NORMAL_DRAW_BUFFER : register(b4)
{
	int normalType;
}

// most verts we export is 3 lines for each vert and 1 line for the face
// 3 * 3 + 2 = 8
[maxvertexcount(8)]
void main(
	triangle VOut input[3], 
	inout LineStream< VOut > output
)
{
	// used to scale down the length of normal lines
	float denom = 8;

	// find normal of face
	float3 ab = input[0].pixelPos - input[1].pixelPos;
	float3 ac = input[2].pixelPos - input[1].pixelPos;
	float3 norm = cross(ac, ab);
	norm = normalize(norm);

	// find center of face
	float3 center = (input[0].pixelPos + input[1].pixelPos + input[2].pixelPos) / 3.0f;

	// draw face normal from center
	VOut vert1;
	vert1.position = mul(Projection, float4(center, 1));
	vert1.pixelPos = center;
	vert1.normal = norm;
	vert1.color = float4(0, 0, 0, 0);

	VOut vert2;
	vert2.position = mul(Projection, float4(center + norm/denom, 1));
	vert2.pixelPos = center;
	vert2.normal = norm;
	vert2.color = float4(0,0,0,0);

	// if draw face normals is on add verts to output
	if (normalType & 1)
	{
		output.Append(vert1);
		output.Append(vert2);
		output.RestartStrip();
	}

	// draw a normal line from each vertex
	for (int i = 0; i < 3; ++i)
	{
		VOut vert;
		vert.position = mul(Projection, float4(input[i].pixelPos + normalize(input[i].normal)/denom, 1));
		vert.pixelPos = input[i].pixelPos;
		vert.normal = input[i].normal;
		vert.color = float4(0, 0, 0, 0);

		input[i].color.a = 0;

		// if draw vertex normals is on, add verts to output
		if (normalType & 2)
		{
			output.Append(input[i]);
			output.Append( vert );
			output.RestartStrip();
		}
	}
}