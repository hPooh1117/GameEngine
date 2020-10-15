#include "Application.h"
#include <algorithm>
#include <deque>

#include "Input.h"

#include "./Renderer/GraphicsEngine.h"

#include "./Engine/GameSystem.h"
#include "./Engine/MainCamera.h"
#include "./Engine/Settings.h"
#include "./Engine/Singleton.h"

#include "./Utilities/ImguiSelf.h"
#include "./Utilities/Log.h"


using namespace DirectX;


Application::Application()
{
	
}

bool Application::Init()
{
#ifdef _DEBUG
	Settings::Logger logger = {true, false};
#else
	Settings::Logger logger = { false, false };
#endif



	Log::Initialize(logger);

	Window::Init();

	m_pGraphicsEngine = std::make_unique<GraphicsEngine>();


	if (m_pGraphicsEngine->Initialize(mHwnd))
	{
		Log::Info("[RENDERER] DirectX11 is Initialized.");
	}

	//m_pGameSystem = std::make_unique<GameSystem>(m_pGraphicsEngine->GetDevicePtr());
	Singleton<GameSystem>::Get().Initialize(m_pGraphicsEngine->GetDevicePtr());

	return true;

}


void Application::Update(float elapsed_time)
{
	InputPtr->HandleInput(mHwnd);

	//m_pGameSystem->Update(elapsed_time);
	Singleton<GameSystem>::Get().Update(elapsed_time);

	

	//Input::Update();
}

void Application::Render(float elapsed_time)
{
	m_pGraphicsEngine->BeginRender();
	//m_pGameSystem->RenderCurrentScene(m_pGraphicsEngine, elapsed_time);
	Singleton<GameSystem>::Get().Render(m_pGraphicsEngine, elapsed_time);

	m_pGraphicsEngine->EndRender();
}



Application::~Application()
{
	//Input::UnInit();
	SingletonFinalizer::Finalize();
}
