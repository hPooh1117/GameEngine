#include "SceneManager.h"

//#include "Scene01.h"
//#include "SceneTitle.h"
#include "SceneA.h"
#include "SceneB.h"
#include "SceneC.h"
#include "SceneD.h"
#include "SceneE.h"
#include "SceneShadowMapTest.h"
#include "SceneTest.h"

#include "SceneLoading.h"

#include "./Engine/GameSystem.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Settings.h"

#include "./Utilities/Log.h"
#include "./Utilities/ImGuiSelf.h"


//----------------------------------------------------------------------------------------------------------------------------

SceneManager::SceneManager(D3D::DevicePtr& p_device)
	:mCurrentScene(SceneID::SCENE_B),
	mClearFlag(false),
	m_bIsLoading(false),
	m_pDevice(p_device),
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
		mSceneStack.emplace(std::make_unique<SceneA>(this, m_pDevice));
		break;
	case SceneID::SCENE_B:
		mSceneStack.emplace(std::make_unique<SceneB>(this, m_pDevice));
		break;
	case SceneID::SCENE_C:
		mSceneStack.emplace(std::make_unique<SceneC>(this, m_pDevice));
		break;
	case SceneID::SCENE_D:
		mSceneStack.emplace(std::make_unique<SceneD>(this, m_pDevice));
		break;
	case SceneID::SCENE_E:
		mSceneStack.emplace(std::make_unique<SceneE>(this, m_pDevice));
		break;
	case SceneID::SCENE_F:
		mSceneStack.emplace(std::make_unique<SceneF>(this, m_pDevice));
		break;
	case SceneID::SCENE_TEST:
		mSceneStack.emplace(std::make_unique<SceneTest>(this, m_pDevice));
		break;
	}

}

void SceneManager::InitializeLoadingScene()
{
	Log::Info("Initializing Scene...");
	m_pLoadingScene = std::make_unique<SceneLoading>(this, m_pDevice);
	
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::InitializeCurrentScene()
{
	m_bIsLoading = true;

	CreateScene(mCurrentScene);

	mSceneStack.top()->SetCurrntSceneID(mCurrentScene);
	mSceneStack.top()->InitializeScene();
	Log::Info("Initialized %s", mSceneStack.top()->GetCurrentSceneName().c_str());
	m_bIsLoading = false;
	mbIsInitialized = true;
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::ExecuteLoadingScene(float elapsed_time)
{
	m_pLoadingScene->Update(elapsed_time);
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
	m_bIsLoading = false;
	mbIsInitialized = true;
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::RenderLoadingScene(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	m_pLoadingScene->Render(p_graphics, elapsed_time);
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::RenderCurrentScene(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	mSceneStack.top()->Render(p_graphics, elapsed_time);

}

void SceneManager::RenderUIForCurrentScene()
{
	mSceneStack.top()->RenderUI();
}

void SceneManager::PreComputeForNextScene(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	mSceneStack.top()->PreCompute(p_graphics);
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
		mClearFlag = true;
		mNextScene = nextScene;
	}

	m_bIsLoading = true;
	mbIsInitialized = false;
}

//----------------------------------------------------------------------------------------------------------------------------
