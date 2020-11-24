#include "Application.h"
#include <algorithm>
#include <deque>

#include "Input.h"

#include "./Engine/GameSystem.h"
#include "./Engine/Settings.h"
#include "./Engine/Singleton.h"

#include "./Renderer/RenderTarget.h"

#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Swapchain.h"

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

	Window::ShowAndUpdateWindow();

	//m_pGameSystem = std::make_unique<GameSystem>(m_pGraphicsEngine->GetDevicePtr());
	ENGINE.Initialize(m_pGraphicsEngine);

	return true;

}


void Application::Update(float elapsed_time)
{
	InputPtr.HandleInput(mHwnd);

	//m_pGameSystem->Update(elapsed_time);
	ENGINE.Update(m_pGraphicsEngine, elapsed_time);

	

	//Input::Update();
}

void Application::Render(float elapsed_time)
{
	//m_pGraphicsEngine->BeginRender();
	//m_pGameSystem->RenderCurrentScene(m_pGraphicsEngine, elapsed_time);
	ENGINE.Render(m_pGraphicsEngine, elapsed_time);

	m_pGraphicsEngine->EndRender();
}

void Application::OnSize()
{
	RECT rc = this->GetClientWindowRect();
	m_pGraphicsEngine->GetSwapchain()->Resize(m_pGraphicsEngine->GetDevicePtr(), rc.right - rc.left, rc.bottom - rc.top);

	ENGINE.SetCurrentWindowSize(m_pGraphicsEngine->GetDevicePtr(), rc.right - rc.left, rc.bottom - rc.top);
}



Application::~Application()
{
	//Input::UnInit();
	SingletonFinalizer::Finalize();
}
