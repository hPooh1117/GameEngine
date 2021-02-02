#pragma once
#include "RenderPasses.h"

class RenderTarget;
class Sprite;

enum GBufferID
{
	EColor,
	ENormal,
	EPosition,
	EShadow,
	EDepth,

	EDiffuse,
	ESpecular,
	ESkyboxPost,
	EPreLighting,

	EResult,
	EEffectSSAO,

	ESecondaryResult,
	EBlurredAO,

	ENUM_GBUFFER_ID_MAX,
};

class DeferredPass : public RenderPass
{
public:
	static const char* GBUFFER_NAME[ENUM_GBUFFER_ID_MAX];
	static const UINT GEOMETRY_BUFFER_SIZE = 5;
	static const UINT LIGHT_BUFFER_SIZE = 4;
private:
	std::unique_ptr<RenderTarget> mpPreLightingTarget;
	std::unique_ptr<Sprite> mpScreen;

public:
	DeferredPass();
	virtual ~DeferredPass() = default;

	virtual void Initialize(Graphics::GraphicsDevice* p_device) override;

	void InitializeGBuffer(Graphics::GraphicsDevice* p_device);
	void RenderDefferedLighting(Graphics::GraphicsDevice* p_graphics, float elapsed_time);
	void RenderUI(bool b_open);
	void RenderUIForAnotherScreen();

	const std::unique_ptr<RenderTarget>& GetPreLightingTargetPtr() { return mpPreLightingTarget; }

};

