#include "Application.h"
#include <algorithm>
#include <deque>

#include "Input.h"

#include "./Renderer/GraphicsEngine.h"

#include "./Engine/GameSystem.h"
#include "./Engine/MainCamera.h"
#include "./Engine/Settings.h"

#include "./Utilities/ImguiSelf.h"
#include "./Utilities/Log.h"


using namespace DirectX;


Application::Application()
{

}

bool Application::Init()
{
	Settings::Logger logger = {true, false};

	Log::Initialize(logger);

	Window::Init();

	m_pGraphicsEngine = std::make_unique<GraphicsEngine>();

	//Input::AddListener(this);
	//Input::ShowCursor(false);

	if (m_pGraphicsEngine->Initialize(mHwnd))
	{
		Log::Info("[RENDERER] DirectX11 is Initialized.");
	}

	m_pGameSystem = std::make_unique<GameSystem>(m_pGraphicsEngine->GetDevicePtr());


	return true;

}


void Application::Update(float elapsed_time)
{
	InputPtr->HandleInput(mHwnd);

	m_pGameSystem->update(elapsed_time);

	

	//Input::Update();
}

void Application::Render(float elapsed_time)
{
	m_pGraphicsEngine->BeginRender();
	m_pGameSystem->Render(m_pGraphicsEngine, elapsed_time);
	m_pGraphicsEngine->EndRender();
}



Application::~Application()
{
	//Input::UnInit();
}
