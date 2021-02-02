#include "Application.h"
#include <algorithm>
#include <deque>

#include "Input.h"


#include "./Engine/GameSystem.h"
#include "./Engine/Settings.h"
#include "./Engine/Singleton.h"
#include "./Engine/TestEngine.h"

#include "./Renderer/RenderTarget.h"

#include "./Renderer/Swapchain.h"

#include "./Utilities/Log.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "libfbxsdk-md.lib")

#else
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "libfbxsdk-md.lib")

#endif

//#define IMPLEMENT_ECS

using namespace DirectX;


Application::Application()
{
	
}

bool Application::Initialize()
{
#ifdef _DEBUG
	Settings::Logger logger = {true, false};
#else
	Settings::Logger logger = { false, false };
#endif



	Log::Initialize(logger);
	
	Window::Initialize();
	

	mpGraphicsDevice = std::make_unique<Graphics::GraphicsDevice>();

	if (mpGraphicsDevice->Initialize(mHwnd))
	{
		Log::Info("[RENDERER] DirectX11 is Initialized.");
	}

	Window::ShowAndUpdateWindow();


#ifndef IMPLEMENT_ECS
	ENGINE.Initialize(mpGraphicsDevice);
#else 
	TEST.Initialize();
#endif // IMPLEMENT_ECS
	return true;

}


void Application::Update(float elapsed_time)
{
	


	InputPtr.HandleInput(mHwnd);

#ifndef IMPLEMENT_ECS
	ENGINE.Update(mpGraphicsDevice, elapsed_time);
#else 
	TEST.Update(elapsed_time);
#endif
	

	//Input::Update();
}

void Application::Render(float elapsed_time)
{

#ifndef IMPLEMENT_ECS
	ENGINE.Render(mpGraphicsDevice, elapsed_time);
#else 
	m_pGraphicsEngine->BeginRender();
	TEST.Render();
#endif //IMPLEMENT_ECS
	mpGraphicsDevice->EndRender();
}

void Application::OnSize()
{
	RECT rc = this->GetClientWindowRect();
	mpGraphicsDevice->GetSwapchain()->Resize(mpGraphicsDevice->GetDevicePtr(), rc.right - rc.left, rc.bottom - rc.top);

#ifndef IMPLEMENT_ECS
	ENGINE.SetCurrentWindowSize(mpGraphicsDevice->GetDevicePtr(), rc.right - rc.left, rc.bottom - rc.top);
#endif //IMPLEMENT_ECS
}



Application::~Application()
{
	//Input::UnInit();
	SingletonFinalizer::Finalize();
}
