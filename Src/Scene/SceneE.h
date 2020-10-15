#pragma once
#include "Scene.h"
#include <memory>
#include <DirectXMath.h>


class ShadowMap;
class AmbientOcclusion;
class MultiRenderTarget;

class SceneE : public Scene
{
public:
	SceneE(SceneManager* manager, D3D::DevicePtr& device);

	virtual void InitializeScene() override;

	virtual void Update(float elapsed_time) override;
	void         RegistUIClients();
	virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;
	~SceneE();

private:
	void InitializeActors();


private:
	std::shared_ptr<Actor> m_pPlayer;
	std::shared_ptr<Actor> m_pCat;
	std::shared_ptr<Actor> m_pOswell;
	std::shared_ptr<Actor> m_pShiba;
	std::shared_ptr<Actor> m_pField;

private:
	std::unique_ptr<ShadowMap> m_pShadowMap;
	std::shared_ptr<AmbientOcclusion> m_pAO;
	std::shared_ptr<MultiRenderTarget> m_pMRT;

	// Shadow Map(Forward)
	std::shared_ptr<Shader> m_pToShadowShader;
	std::shared_ptr<Shader> m_pFromShadowShader;
	std::shared_ptr<Shader> m_pFromShadow;
	std::shared_ptr<Shader> m_pToShadow;
	std::shared_ptr<Shader> m_pFromShadowForMotion;
	std::shared_ptr<Shader> m_pToShadowForMotion;

	// MRT(Deffered)
	std::shared_ptr<Shader> m_pFromGBuffer;
	std::shared_ptr<Shader> m_pToGBuffer;
	std::shared_ptr<Shader> m_pToGBufferForSkinnedMesh;
	std::shared_ptr<Shader> m_pToGBufferForSkinning;

	std::shared_ptr<Shader> m_pToGBufferForBump;
	std::shared_ptr<Shader> m_pToGBufferSkinnedMeshForBump;
	std::shared_ptr<Shader> m_pDefferedLightShader;
	std::shared_ptr<Shader> m_pScreenShader;
};
