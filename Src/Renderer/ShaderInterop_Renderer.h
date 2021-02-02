#pragma once
#include "ShaderInterop.h"

CBUFFER(CBufferForMesh, CBUFFER_MESH)
{
	float4x4 g_WVP;
	float4x4 g_World;
	float4x4 g_Inv_View_Proj;
	float4x4 g_Inv_View;
	float4x4 g_Inv_Proj;
	float4 g_Param = {};
};

CBUFFER(CBufferForMaterial, CBUFFER_MATERIAL)
{
	float4 g_Material_Color;
	float3 g_Specular_Color;
	float g_Metalness;
	float g_Roughness;
	float g_Diffuse_Factor = 0.0f;
	float g_Specular_Factor = 0.0f;
	int   g_Texture_Config;
};