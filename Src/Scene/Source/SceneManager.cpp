#include "SceneManager.h"

#include "Scene01.h"
#include "SceneTitle.h"
#include "SceneA.h"
#include "SceneB.h"
#include "SceneC.h"
#include "SceneD.h"
#include "SceneE.h"
#include "SceneShadowMapTest.h"
#include "SceneLoading.h"
#include "./Utilities/Log.h"


//----------------------------------------------------------------------------------------------------------------------------

SceneManager::SceneManager(D3D::DevicePtr& p_device)
	:mCurrentScene(SceneID::SCENE_E),
	mNextScene(SceneID::SCENE_E),
	mClearFlag(false),
	m_pDevice(p_device)
{

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

void SceneManager::InitializeLoadingScene()
{
	Log::Info("Initializing Scene...");
	m_pLoadingScene = std::make_unique<SceneLoading>(this, m_pDevice);

}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::InitializeCurrentScene()
{
	mSceneStack.emplace(std::make_unique<SceneE>(this, m_pDevice));
	mSceneStack.top()->SetSceneID(mCurrentScene);
	mSceneStack.top()->InitializeScene();
	Log::Info("Initialized %s", mSceneStack.top()->GetSceneName().c_str());

}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::ExecuteLoadingScene(float elapsed_time)
{
	m_pLoadingScene->Update(elapsed_time);
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::ExecuteCurrentScene(float elapsed_time)
{
	mSceneStack.top()->ChangeScene();

	mSceneStack.top()->Update(elapsed_time);
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

//----------------------------------------------------------------------------------------------------------------------------

void SceneManager::ChangeScene(const SceneID& nextScene, bool clearCurrentScene)
{
	if (clearCurrentScene == true)
	{
		mClearFlag = true;
		mSceneStack.pop();
		mNextScene = nextScene;
	}

	Log::Info("Initializing Scene...");

	switch (nextScene)
	{
	case SceneID::SCENE01:
		mSceneStack.emplace(std::make_unique<Scene01>(this, m_pDevice));
		break;
	case SceneID::SCENETITLE:
		mSceneStack.emplace(std::make_unique<SceneTitle>(this, m_pDevice));
		break;
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
	}
	mSceneStack.top()->SetSceneID(nextScene);
	Log::Info("Initialized %s", mSceneStack.top()->GetSceneName().c_str());

}

//----------------------------------------------------------------------------------------------------------------------------
