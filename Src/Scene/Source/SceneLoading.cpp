#include "SceneLoading.h"

#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/LightController.h"
#include "./Engine/MeshRenderer.h"

#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Skybox.h"

//#include "./Renderer/GraphicsDevice.h"
#include "./Renderer/Sprite.h"
//#include "./Renderer/Shader.h"
//#include "./Renderer/VertexDecleration.h"

SceneLoading::SceneLoading(SceneManager* p_manager, Graphics::GraphicsDevice* p_device) :Scene(p_manager, p_device)
{
	//mpSprScreen = std::make_shared<Sprite>(p_device, L"./Data/Images/Loading/waiya_w.png");

	mpSprGene = std::make_unique<Sprite>(p_device, L"./Data/Images/Loading/rotate_gene_shrinked.png");

	mpSprLoadingString = std::make_unique<Sprite>(p_device, L"./Data/Images/Loading/loading_bk.png");


	m_pBasicQuad = std::make_shared<Shader>();
	m_pBasicQuad->createShader(
		p_device->GetDevicePtr(),
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

void SceneLoading::PreCompute(Graphics::GraphicsDevice* p_graphics)
{
}

void SceneLoading::Render(Graphics::GraphicsDevice* p_device, float elapsed_time)
{
	static unsigned int timer = 0;

	D3D::DeviceContextPtr immContext = p_device->GetImmContextPtr();

	p_device->OMSetDepthStencilState(Graphics::DS_FALSE);

	p_device->mBlender.SetBlendState(immContext, Blender::BLEND_ALPHA);

	p_device->ActivateBackBuffer();

	static float angle = 0.0f;
	angle += 30.0f * elapsed_time;

	unsigned int frame = timer / 5;




	mpSprGene->Render(
		p_device,
		m_pBasicQuad.get(),
		Vector2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f),
		Vector2(1280.0f, 720.0f),
		//Vector2(timer % 6 * 1280.0f + 2.0f, timer / 10 * 720),
		Vector2(frame % 15 * 640.0f, frame / 15 * 360.0f),
		//Vector2(0, 0),
		Vector2(640.0f, 360.0f), .0f, Vector4(1, 1, 1, 1));


	mpSprLoadingString->Render(
		p_device,
		m_pBasicQuad.get(),
		Vector2(SCREEN_WIDTH * 0.7f, SCREEN_HEIGHT * 0.73f),
		Vector2(640.0f, 360.0f),
		//Vector2(timer % 6 * 1280.0f + 2.0f, timer / 10 * 720),
		Vector2(0, 0),
		//Vector2(0, 0),
		Vector2(1280.0f, 720.0f), .0f, Vector4(1, 1, 1, 1));


	timer++;
	if (frame >= 59) timer = 0;
}

void SceneLoading::RenderUI()
{
}

