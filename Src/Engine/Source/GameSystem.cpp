#include "GameSystem.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Scene/SceneManager.h"

#include "./Utilities/Log.h"
#include "./Utilities/PerfTimer.h"

GameSystem::GameSystem(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
    m_pTimer = std::make_unique<PerfTimer>();

    
    Log::Info("[ENGINE] Initializing ----------------------------------------------");
    
    m_pTimer->Start();

    m_scene_manager = std::make_unique<SceneManager>(device);

    m_pTimer->Stop();


    Log::Info("[ENGINE] Initialized ( %.2f s )-------------------------------------", m_pTimer->GetDeltaTime());
}

void GameSystem::update(float elapsed_time)
{
    m_scene_manager->Execute(elapsed_time);
}

void GameSystem::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
    m_scene_manager->Render(p_graphics, elapsed_time);
}


GameSystem::~GameSystem()
{
}
