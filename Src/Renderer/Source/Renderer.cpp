#include "Renderer.h"
#include "./Application/ThreadPool.h"
//#include "./RenderPass/ForwardPasses.h"
//#include "./RenderPass/DeferredPasses.h"
//#include "./RenderPass/ShadowPasses.h"
//#include "./RenderPass/MakeCubeMapPasses.h"
//#include "./RenderPass/SSAOPasses.h"
//#include "./RenderPass/PostProcessPasses.h"
//#include "GraphicsDevice.h"
//#include "NewMeshRenderer.h"
//#include "./Engine/UIRenderer.h"
#include "./Engine/GameSystem.h"
#include "./Engine/CameraController.h"
#include "./Engine/LightController.h"
#include "./Scene/SceneManager.h"
//#include "./Engine/Settings.h"
//#include "TextureHolder.h"
#include "Sprite.h"
#include "./Utilities/Log.h"

Renderer::Renderer()
	:mpForwardPass(std::make_unique<ForwardPass>()),
	mpDefferedPass(std::make_unique<DeferredPass>()),
	mpShadowPass(std::make_unique<ShadowPass>()),
	mpCubeMapPass(std::make_unique<MakeCubeMapPass>()),
	mpSSAOPass(std::make_unique<SSAOPass>()),
	mpPostProcessPass(std::make_unique<PostProcessPass>()),
	mpMeshRenderer(std::make_unique<NewMeshRenderer>()),
	mpUIRenderer(nullptr),
	mpTextureHolder(std::make_unique<TextureHolder>())
{

}

void Renderer::SetUp(const Settings::Renderer& setting)
{
	mbCastingShadow = setting.bCastShadow;
	mbCubeMap = setting.bCubeMap;
	mbDeferredRendering = setting.bIsDeferred;
	mbSSAOActivated = setting.bEnableAO;
}

void Renderer::Initialize(Graphics::GraphicsDevice* device)
{
	D3D::DevicePtr p_device = device->GetDevicePtr();

	mpUIRenderer = std::make_unique<UIRenderer>(device);

	mpMeshRenderer->Initialize(device);

}

void Renderer::InitializeAsync(Graphics::GraphicsDevice* device)
{
	D3D::DevicePtr& pDevice = device->GetDevicePtr();

	mbLoading = true;

	auto AsyncLoadRenderPasses = [=]()
	{
		Log::Info("[ENGINE] Start Loading RenderPasses(Async)");

		PerfTimer timer;
		timer.Start();
		RenderPass::Clear();
		{
			std::unique_lock<std::mutex> lck(ENGINE.mLoadingMutex);
			mpPostProcessPass->Initialize(device);
		}
		{
			std::unique_lock<std::mutex> lck(ENGINE.mLoadingMutex);
			if (mbCastingShadow) mpShadowPass->Initialize(device);
		}
		{
			std::unique_lock<std::mutex> lck(ENGINE.mLoadingMutex);
			if (!mbDeferredRendering) mpForwardPass->Initialize(device);
			else               mpDefferedPass->Initialize(device);
		}
		{
			std::unique_lock<std::mutex> lck(ENGINE.mLoadingMutex);
			if (mbDeferredRendering) mpSSAOPass->Initialize(device);
		}
		{
			std::unique_lock<std::mutex> lck(ENGINE.mLoadingMutex);
			if (mbCubeMap) mpCubeMapPass->Initialize(device);
		}
		//{
		//    std::unique_lock<std::mutex> lck(mLoadingMutex);
		//    mpFinalPass->Create(pDevice);
		//}

		mbLoading = false;
		mbIsInitialized = true;

		timer.Stop();
		Log::Info("[ENGINE] Loaded RenderPasses(Async) in %.2f(s)---------------", timer.GetDeltaTime());
		return true;
	};

	ENGINE.GetThreadPoolPtr()->Enqueue(AsyncLoadRenderPasses);

}

void Renderer::Update(Graphics::GraphicsDevice* device)
{
	mpUIRenderer->UpdateFrame();
}

void Renderer::Render(Graphics::GraphicsDevice* device, float dt)
{
	if (mbCastingShadow) mpShadowPass->RenderShadow(device, dt);
	if (mbCubeMap)       mpCubeMapPass->MakeCubeMap(device, dt);
	if (!mbDeferredRendering)     mpForwardPass->RenderForwardLighting(device, dt);
	if (mbDeferredRendering)      mpDefferedPass->RenderDefferedLighting(device, dt);

	if (mbSSAOActivated)          mpSSAOPass->ExecuteSSAO(device, dt);

	mpPostProcessPass->RenderPostProcess(device, dt);

	ENGINE.GetSceneManager()->RenderCurrentScene(device, dt);

	RenderUI(device, dt);
}

void Renderer::Clear()
{
	mbIsInitialized = false;
	mpTextureHolder->ClearTextureTable();
	mpUIRenderer->ClearUIClients();
	mpMeshRenderer->ClearAll();
}

void Renderer::RenderUI(Graphics::GraphicsDevice* device, float dt)
{
	if (mpUIRenderer->GetUIEnable())
	{
		RenderUIForSettings(device, dt);

		if (mbDeferredRendering) mpDefferedPass->RenderUIForAnotherScreen();
		if (mbSSAOActivated)     mpSSAOPass->RenderUIForAnotherScreen();

		RenderUIForScene(device, dt);

	}

	RenderUIForMainMenuBar(device);


	mpUIRenderer->Execute();

	device->mBlender.SetBlendState(device->GetImmContextPtr(), Blender::BLEND_ADD);

	mpUIRenderer->RenderSpriteFontQueue(device);

	device->mBlender.SetBlendState(device->GetImmContextPtr(), Blender::BLEND_ALPHA);

}


void Renderer::CheckShaderActivated()
{
	mpForwardPass->CheckActivatedShaders();
	mpDefferedPass->CheckActivatedShaders();
	mpShadowPass->CheckActivatedShaders();
	mpSSAOPass->CheckActivatedShaders();
	mpPostProcessPass->CheckActivatedShaders();

}

void Renderer::RenderUIForSettings(Graphics::GraphicsDevice* device, float elapsed_time)
{
	mpUIRenderer->SetNextWindowSettings(Vector2(SCREEN_WIDTH - 350, 23), Vector2(350, 337));
	mpUIRenderer->SetNextUIConfig(true);
	mpUIRenderer->BeginRenderingNewWindow("RenderTatgets");

	RenderUIByRenderPasses(device);
	mpUIRenderer->FinishRenderingWindow();



	mpUIRenderer->SetNextWindowSettings(Vector2(SCREEN_WIDTH - 350, 360), Vector2(350, SCREEN_HEIGHT - 360));
	mpUIRenderer->BeginRenderingNewWindow("Settings");

	if (mbDeferredRendering) ImGui::Checkbox("Enable SSAO", &mbSSAOActivated);
	if (mbDeferredRendering) mpSSAOPass->RenderUIForSettings();
	ENGINE.GetCameraPtr()->RenderUI();
	ENGINE.GetLightPtr()->RenderUI();
	mpMeshRenderer->RenderUI();
	mpPostProcessPass->RenderUIForSettings();

	//ImGui::Text("Elapsed Time on Update : %.5f ms", mFrameTimer);
	device->RenderUI();

	mpUIRenderer->FinishRenderingWindow();

}

void Renderer::RenderUIByRenderPasses(Graphics::GraphicsDevice* device)
{
	bool bOpen = true;

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Tab"))
		{
			if (ImGui::MenuItem("Close All Tabs"))
			{
				bOpen = false;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (mbCastingShadow) mpShadowPass->RenderUI(bOpen);
	if (mbCubeMap)       mpCubeMapPass->RenderUI(bOpen);
	if (!mbDeferredRendering)     mpForwardPass->RenderUI(bOpen);
	if (mbDeferredRendering)      mpDefferedPass->RenderUI(bOpen);
	if (mbSSAOActivated)          mpSSAOPass->RenderUI(bOpen);
	mpPostProcessPass->RenderUI();

}

void Renderer::RenderUIForMainMenuBar(Graphics::GraphicsDevice* device)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (mbCastingShadow) mpShadowPass->ShowShaderList(device, "ShadowPass");
			if (mbCubeMap)       mpCubeMapPass->ShowShaderList(device, "CubemapPass");
			if (!mbDeferredRendering)     mpForwardPass->ShowShaderList(device, "ForwardPass");
			if (mbDeferredRendering)      mpDefferedPass->ShowShaderList(device, "DefferedPass");
			if (mbSSAOActivated)          mpSSAOPass->ShowShaderList(device, "SSAOPass");
			mpPostProcessPass->ShowShaderList(device, "PostProcessPass");

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Enable UI"))
			{
				mpUIRenderer->SetUIEnable(true);
			}
			if (ImGui::MenuItem("Disable UI"))
			{
				mpUIRenderer->SetUIEnable(false);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	mpShadowPass->ReloadShaderFile(device);
	mpCubeMapPass->ReloadShaderFile(device);
	mpForwardPass->ReloadShaderFile(device);
	mpDefferedPass->ReloadShaderFile(device);
	mpSSAOPass->ReloadShaderFile(device);
	mpPostProcessPass->ReloadShaderFile(device);

}

void Renderer::RenderUIForScene(Graphics::GraphicsDevice* device, float elapsed_time)
{
	mpUIRenderer->SetNextWindowSettings(Vector2(0, 23), Vector2(150, 400));
	mpUIRenderer->SetNextUIConfig(true);
	mpUIRenderer->BeginRenderingNewWindow("Scene", false);

	ENGINE.GetSceneManager()->RenderUI();


	mpUIRenderer->FinishRenderingWindow();

	ENGINE.GetSceneManager()->RenderUIForCurrentScene();

}


RenderPass* Renderer::GetRenderPass(unsigned int render_pass_id)
{
		switch (render_pass_id)
		{
		case RenderPassID::EForwardPass:
			return mpForwardPass.get();
		case RenderPassID::EShadowPass:
			return mpShadowPass.get();
		case RenderPassID::EDeferredPass:
			return mpDefferedPass.get();
		case RenderPassID::EPostEffectPass:
			return mpPostProcessPass.get();
		case RenderPassID::ESSAOPass:
			return mpSSAOPass.get();
		case RenderPassID::ECubeMapPass:
			return mpCubeMapPass.get();
		default:
			return mpForwardPass.get();
			break;
		}
}

NewMeshRenderer* Renderer::GetMeshRenderer()
{
	return mpMeshRenderer.get();
}

UIRenderer* Renderer::GetUIRenderer()
{
	return mpUIRenderer.get();
}

TextureHolder* Renderer::GetTextureHolderPtr()
{
	return mpTextureHolder.get();
}
