#pragma once
enum ShaderID : unsigned int
{
	// CommonShader
	ESprite,
	ESkybox,
	ESkyboxRevised,

	// ForwardPasses
	ELambert,
	ELambertSkinned,
	EPhong,
	ELine,

	EEnvironmentMap,
	ENormalMap,

	EToonGeo,
	EForwardPBR,
	EForwardPBRForSkinned,
	ESpotLightPhong,
	ESpotLightPhongForSkinning,
	ESpotLightBump,
	ESpotLightOcean,

	EFromShadow,
	EFromShadowForSkinning,
	ETextureTest,
	EFlat,
	EFurShader,

	ECubemapConvolution,
	EMakeCubeMap,
	EMakeCubeMapForSkinning,
	EUseCubeMap,
	EReflectSea, // using cubemap
	ECubeMapEnv,

	// DeferredPass
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
	EDefferedPreLighting, // POINT + DIRECTIONAL
	EDefferedSea,
	EDefferedLighting, // POINT + SPOT + DIRECTIONAL
	EDefferedSkyboxRevised,

	// ShadowPasses
	EToShadow,
	EToShadowForSkinning,

	// SSAOPasses
	ESSAOCompute,
	ESSAOPixel,
	EBlur,
	EAlchemyAO,

	// PostProcessPasses
	EPostEffect,
	ECS_Desaturate,
	ECS_GaussianHorizontal,
	ECS_GaussianVertical,

	ENUM_SHADER_MAX,

	UNREGISTERED_SHADER = UINT32_MAX
};
