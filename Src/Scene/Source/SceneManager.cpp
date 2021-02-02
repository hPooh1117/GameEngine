#include "SceneManager.h"

//#include "Scene01.h"
//#include "SceneTitle.h"
#include "SceneA.h"
#include "SceneB.h"
#include "SceneC.h"
#include "SceneD.h"
#include "SceneE.h"
#include "SceneCubeMapTest.h"
#include "SceneTest.h"

#include "SceneLoading.h"

#include "./Engine/GameSystem.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Settings.h"

#include "./Utilities/Log.h"
#include "./Utilities/ImGuiSelf.h"


//----------------------------------------------------------------------------------------------------------------------------

SceneManager::SceneManager(Graphics::GraphicsDevice* p_device)
	:mCurrentScene(SceneID::SCENE_A),
	mbClearFlag(false),
	mbIsLoading(false),
	mpGraphicsDevice(p_device),
	mbIsInitialized(false)
{
	mNextScene = mCurrentScene;
}

//----------------------------------------------------------------------------------------------------------------------------

SceneManager::~SceneManager()
{
	while (!mSceneStack.empty())
	{
		mSceneStack.pop();
	}
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::CreateScene(SceneID scene_id)
{
	switch (static_cast<SceneID>(scene_id))
	{
		//case SceneID::SCENE01:
		//	mSceneStack.emplace(std::make_unique<Scene01>(this, m_pDevice));
		//	break;
		//case SceneID::SCENETITLE:
		//	mSceneStack.emplace(std::make_unique<SceneTitle>(this, m_pDevice));
		//	break;
	case SceneID::SCENE_A:
		mSceneStack.emplace(std::make_unique<SceneA>(this, mpGraphicsDevice));
		break;
	case SceneID::SCENE_B:
		mSceneStack.emplace(std::make_unique<SceneB>(this, mpGraphicsDevice));
		break;
	case SceneID::SCENE_C:
		mSceneStack.emplace(std::make_unique<SceneC>(this, mpGraphicsDevice));
		break;
	case SceneID::SCENE_D:
		mSceneStack.emplace(std::make_unique<SceneD>(this, mpGraphicsDevice));
		break;
	case SceneID::SCENE_E:
		mSceneStack.emplace(std::make_unique<SceneE>(this, mpGraphicsDevice));
		break;
	case SceneID::SCENE_F:
		mSceneStack.emplace(std::make_unique<SceneF>(this, mpGraphicsDevice));
		break;
	case SceneID::SCENE_TEST:
		mSceneStack.emplace(std::make_unique<SceneTest>(this, mpGraphicsDevice));
		break;
	}

}

void SceneManager::InitializeLoadingScene()
{
	Log::Info("Initializing Scene...");
	mpLoadingScene = std::make_unique<SceneLoading>(this, mpGraphicsDevice);
	
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::InitializeCurrentScene()
{
	mbIsLoading = true;

	CreateScene(mCurrentScene);

	mSceneStack.top()->SetCurrntSceneID(mCurrentScene);
	mSceneStack.top()->InitializeScene();
	Log::Info("Initialized %s", mSceneStack.top()->GetCurrentSceneName().c_str());
	mbIsLoading = false;
	mbIsInitialized = true;
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::ExecuteLoadingScene(float elapsed_time)
{
	mpLoadingScene->Update(elapsed_time);
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::ExecuteCurrentScene(float elapsed_time)
{

	mSceneStack.top()->Update(elapsed_time);

	mSceneStack.top()->ChangeScene();

}

void SceneManager::LoadNextScene()
{
	// ‘O‚ÌƒV[ƒ“‚ð”jŠü
	mSceneStack.pop();


	Log::Info("Initializing Scene...");
	
	CreateScene(mNextScene);

	mSceneStack.top()->SetCurrntSceneID(mNextScene);
	mSceneStack.top()->InitializeScene();
	Log::Info("Initialized %s", mSceneStack.top()->GetCurrentSceneName().c_str());
	mbIsLoading = false;
	mbIsInitialized = true;
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::RenderLoadingScene(Graphics::GraphicsDevice* device, float elapsed_time)
{
	mpLoadingScene->Render(device, elapsed_time);
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::RenderCurrentScene(Graphics::GraphicsDevice* device, float elapsed_time)
{
	mSceneStack.top()->Render(device, elapsed_time);

}

void SceneManager::RenderUIForCurrentScene()
{
	mSceneStack.top()->RenderUI();
}

void SceneManager::PreComputeForNextScene(Graphics::GraphicsDevice* device)
{
	mSceneStack.top()->PreCompute(device);
}

void SceneManager::RenderUI()
{
	using namespace ImGui;

	
	for (auto i = 0u; i < static_cast<unsigned int>(SceneID::SCENE_NUM_MAX); ++i)
	{
		if (Button(mSceneStack.top()->GetSceneName(i).c_str(), ImVec2(110, 30)))
		{
			mSceneStack.top()->SetNextSceneID(static_cast<SceneID>(i));
			mSceneStack.top()->ToggleChangeFlag();
		}
	}
	Text("*** Current Scene ***");
	Text("%s", mSceneStack.top()->GetCurrentSceneName().c_str());

}



//----------------------------------------------------------------------------------------------------------------------------

const Settings::Renderer& SceneManager::GetNextSceneSettings()
{
	return  Scene::mSettings[static_cast<UINT>(mNextScene)];
}

void SceneManager::ChangeScene(const SceneID& nextScene, bool clearCurrentScene)
{
	if (clearCurrentScene == true)
	{
		mbClearFlag = true;
		mNextScene = nextScene;
	}

	mbIsLoading = true;
	mbIsInitialized = false;
}

//----------------------------------------------------------------------------------------------------------------------------
