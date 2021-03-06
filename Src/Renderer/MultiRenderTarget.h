#pragma once
#include <array>
#include <memory>
#include <vector>
#include "./Engine/UIClient.h"
#include "D3D_Helper.h"
class Texture;
class Sprite;
class Shader;
class GraphicsDevice;
class Blender;

class MultiRenderTarget : public UIClient
{
private:
	enum GBufferType : int
	{
		kAlbedo,
		kPosition,
		kNormal,
		kDepth,
		kShadow,

		kDiffuse,
		kSpecular,
		kAmbient,
		kPostEffect,
		kResult,
		kNoAO,
		kNumAll,
	};

	enum ShaderType
	{
		ESimpleQuad,
		EPreLighting,
		EScreen,
	};

public:
	static constexpr UINT NUMBER_OF_GBUFFER = 5;
	static constexpr UINT NUMBER_OF_LIGHTMAP = 3;
	static constexpr UINT NUMBER_OF_SCREEN = 3;

private:
	std::array<D3D::RTVPtr, NUMBER_OF_GBUFFER> m_pRTVs;
	std::array<D3D::RTVPtr, NUMBER_OF_LIGHTMAP> m_pRTVsForLights;
	D3D::RTVPtr m_pRTVForLight;
	std::array<D3D::RTVPtr, NUMBER_OF_SCREEN> m_pRTVForScreens;
	D3D::DSVPtr m_pDSV;
	D3D::Texture2DPtr m_pTexForDSV;

	std::unique_ptr<Texture> m_pColorMap;
	std::unique_ptr<Texture> m_pPositionMap;
	std::unique_ptr<Texture> m_pNormalMap;
	std::unique_ptr<Texture> m_pShadowMap;
	std::unique_ptr<Texture> m_pDepthMap;

	std::unique_ptr<Texture> m_pLightMap;

	std::unique_ptr<Sprite>  m_pColor;
	std::vector<std::shared_ptr<Shader>> m_pShaders;

	std::unique_ptr<Texture> m_pDiffuseLightMap;
	std::unique_ptr<Texture> m_pSpecularLightMap;
	std::unique_ptr<Texture> m_pAmbientLightMap;

	std::unique_ptr<Texture> m_pScreenMap;
	std::unique_ptr<Texture> m_pPostEffectMap;
	std::unique_ptr<Texture> m_pNoAO;

	int                      mCurrentType;
public:
	MultiRenderTarget(
		D3D::DevicePtr& p_device);

	void ActivateGBuffer(std::unique_ptr<GraphicsDevice>& p_graphics);
	void ActivateDefferedLight(std::unique_ptr<GraphicsDevice>& p_graphics);
	void ActivateScreen(std::unique_ptr<GraphicsDevice>& p_graphics);
	void Deactivate(std::unique_ptr<GraphicsDevice>& p_graphics);
	void Render(D3D::DeviceContextPtr& imm_context, std::unique_ptr<Blender>& p_blender);
	void RenderScreen(D3D::DeviceContextPtr& imm_context, std::unique_ptr<Blender>& p_blender);
	void RenderUI() override;
	inline void SetShader(std::shared_ptr<Shader>& p_shader) { m_pShaders.emplace_back(p_shader); }
	~MultiRenderTarget();
};