#pragma once
#include "./Utilities/Vector.h"


static constexpr unsigned int NUM_POINT_LIGHT = 64;
static constexpr unsigned int NUM_SPOT_LIGHT  = 4;

struct PointLightGPU
{
	float index = 0.0f;
	float range = 0.0f;
	float type = 0.0f;
	float dummy = 0.0f;
	Vector4 pos;
	Vector4 color;
};

struct SpotLightGPU
{
	float index = 0.0f;
	float range = 0.0f;
	float type = 0.0f;
	float inner_corn = 0.0f;
	float outer_corn = 0.0f;
	float dummy0 = 0.0f;
	float dummy1 = 0.0f;
	float dummy2 = 0.0f;
	Vector4 pos = {};
	Vector4 color = { 1, 1, 1, 1 };
	Vector4 dir = {};
};

struct DirectionalLightGPU
{
	Vector4 light_color;
	Vector4 light_direction;
	Vector4 ambient_color;
	Vector4 eye_pos;
	float   env_alpha = 0.0f;
	float   refract = 0.0f;
	float   time = 0.0f;
	float   tess_factor = 0.0f;
};

struct DirectionalBRDF
{
	Vector4 light_color;
	Vector4 light_direction;
	Vector4 ambient_color;
	Vector4 eye_pos;
	float   env_alpha = 0.0f;
	float   roughness = 0.0f;
	float   metalness = 0.0f;
	float   time = 0.0f;
	Vector3 specColor;
	float   brdfSpec;
};

struct CBufferForLight
{
	DirectionalLightGPU directionalLight;
	PointLightGPU       pointLight[NUM_POINT_LIGHT];
	SpotLightGPU        spotLight[NUM_SPOT_LIGHT];
};

struct CBufferForPBR
{
	DirectionalBRDF     directionalBRDF;
	PointLightGPU       pointLight[NUM_POINT_LIGHT];
	SpotLightGPU        spotLight[NUM_SPOT_LIGHT];
};

struct MaterialData
{
	DirectX::XMFLOAT4 mat_color;
	DirectX::XMFLOAT3 specularColor;
	float metalness;
	float roughness;
	float diffuse;
	float specular;
	int   textureConfig;
};

