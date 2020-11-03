#pragma once
#include "RenderPasses.h"

class GraphicsEngine;
class RenderTarget;
class Sprite;

enum GBufferID
{
	EColor,
	ENormal,
	EPosition,
	EShadow,
	EDepth,

	EPreLighting,
	EDiffuse,
	ESpecular,
	ESkyboxPost,

	EResult,
	EEffectSSAO,

	ESecondaryResult,
	EBlurredAO,

	ENUM_GBUFFER_ID_MAX,
};

class DefferedPass : public RenderPass
{
public:
	static const char* GBUFFER_NAME[ENUM_GBUFFER_ID_MAX];
	
private:
	std::unique_ptr<RenderTarget> mpPreLightingTarget;
	std::unique_ptr<Sprite> mpScreen;
public:
	DefferedPass();
	virtual ~DefferedPass() = default;

	virtual void Initialize(D3D::DevicePtr& p_device) override;

	void InitializeGBuffer(D3D::DevicePtr& p_device);
	void RenderDefferedLighting(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
	void RenderUI();

	const std::unique_ptr<RenderTarget>& GetPreLightingTargetPtr() { return mpPreLightingTarget; }
};

