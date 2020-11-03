#pragma once
enum ShaderType : unsigned int
{
	// CommonShader
	ESprite,

	// ForwardPasses
	ELambert,
	ELambertSkinned,
	EPhong,
	ELine,
	ESkybox,

	EEnvironmentMap,
	ENormalMap,

	EToonGeo,
	EForwardPBR,
	EForwardPBRSetting,
	ESpotLightPhong,
	ESpotLightPhongForSkinning,
	ESpotLightBump,
	ESpotLightOcean,

	EFromShadow,
	EFromShadowForSkinning,
	ETextureTest,

	// DefferedPass
	EDefferedSkybox,
	EDefferedPhong,
	EDefferedPhongForSkinned,
	EDefferedPhongForSkinning,
	EDefferedShadow,
	EDefferedShadowForSkinned,
	EDefferedShadowForSkinning,
	EDefferedNormal,
	EDefferedNormalForSkinned,
	EDefferedNormalForSkinning,
	EDefferedPreLighting,

	// ShadowPasses
	EToShadow,
	EToShadowForSkinning,
	// SSAOPasses
	ESSAO,
	EBlur,
	// PostProcessPasses
	EPostEffect,

	ENUM_SHADER_MAX,
};
