#pragma once
#include "Scene.h"
#include "./Renderer/D3D_Helper.h"

class ShadowMap;

class SceneF : public Scene
{
public:
public:
	SceneF(SceneManager* manager, D3D::DevicePtr& device);
	virtual void Update(float elapsed_time);
	virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
	~SceneF() = default;

private:
	std::shared_ptr<Actor> m_pPlayer;
	std::shared_ptr<Actor> m_pField;

private:
	std::unique_ptr<ShadowMap> m_pShadowMap;


	// Shadow Map(Forward)
	std::shared_ptr<Shader> m_pToShadowShader;
	std::shared_ptr<Shader> m_pFromShadowShader;
	std::shared_ptr<Shader> m_pToShadowForSkinnedMesh;
	std::shared_ptr<Shader> m_pFromShadowForSkinnedMesh;
	std::shared_ptr<Shader> m_pToShadowForSkinned;
	std::shared_ptr<Shader> m_pFromShadowForSkinned;

};

