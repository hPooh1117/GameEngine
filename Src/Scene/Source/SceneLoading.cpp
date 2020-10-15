#include "SceneLoading.h"

#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/LightController.h"
#include "./Engine/MeshRenderer.h"

#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Skybox.h"

//#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Sprite.h"
//#include "./Renderer/Shader.h"
//#include "./Renderer/VertexDecleration.h"

SceneLoading::SceneLoading(SceneManager* p_manager, D3D::DevicePtr& p_device) :Scene(p_manager, p_device)
{
	m_pScreen = std::make_shared<Sprite>(p_device, L"./Data/Images/LoadIcon2.png");

	m_pBasicQuad = std::make_shared<Shader>();
	m_pBasicQuad->createShader(
		p_device,
		L"./Src/Shaders/sprite_vs.hlsl",
		L"./Src/Shaders/sprite_ps.hlsl",
		"main", "main", VEDType::VED_SPRITE);
}

void SceneLoading::InitializeScene()
{

}

void SceneLoading::Update(float elapsed_time)
{

}

void SceneLoading::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{

	D3D::DeviceContextPtr immContext = p_graphics->GetImmContextPtr();
	m_pBlender->SetBlendState(immContext, Blender::BLEND_ALPHA);

	static float angle = 0.0f;
	angle += 30.0f * elapsed_time;

	m_pScreen->Render(
		immContext,
		m_pBasicQuad,
		Vector2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f),
		Vector2(200.0f, 200.0f),
		Vector2(0.0f, 0.0f),
		Vector2(200.0f, 200.0f), angle, Vector4(1, 1, 1, 1));
}

