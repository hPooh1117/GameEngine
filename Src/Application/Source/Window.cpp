#include "Window.h"

#include <Keyboard.h>

#include "Helper.h"

#include "./Utilities/ImguiSelf.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

using DirectX::Keyboard;

Window::Window()
    :m_bMouseCaptured(false),
    m_bAppWantsExit(false)
{
}

bool Window::Init()
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(nullptr);
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = (L"GraphicsEngine");
    wcex.hIconSm = 0;
    RegisterClassEx(&wcex);

    RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
    mHwnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        L"GraphicsEngine",
        L"", 
        WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, 
        1900 - SCREEN_WIDTH,
        1000 - SCREEN_HEIGHT,
        rc.right - rc.left, 
        rc.bottom - rc.top, 
        NULL, NULL, nullptr, this);
    ShowWindow(mHwnd, SW_SHOW);
    UpdateWindow(mHwnd);


    return true;
}

bool Window::Run()
{
    MSG msg = {};


    m_high_resolution_timer.reset();

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            m_high_resolution_timer.tick();
            calculateFrameStats();

            Update(m_high_resolution_timer.time_interval());
            Render(m_high_resolution_timer.time_interval());
        }
    }

#ifdef USE_IMGUI
    // cleanup imgui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif //USE_IMGUI


    return false;
}

void Window::SetHWND(HWND hwnd)
{
    mHwnd = hwnd;
}

void Window::calculateFrameStats()
{
    // are appended to the window caption bar.
    static int frames = 0;
    static float time_tlapsed = 0.0f;

    frames++;

    // Compute averages over one second period.
    if ((m_high_resolution_timer.time_stamp() - time_tlapsed) >= 1.0f)
    {
        float fps = static_cast<float>(frames); // fps = frameCnt / 1
        float mspf = 1000.0f / fps;
        std::ostringstream outs;
        outs.precision(6);
        outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
        SetWindowTextA(mHwnd, outs.str().c_str());

        // Reset for next average.
        frames = 0;
        time_tlapsed += 1.0f;
    }

}

void Window::startTimer()
{
    m_high_resolution_timer.start();
}

void Window::stopTimer()
{
    m_high_resolution_timer.stop();
}

void Window::CaptureMouse(bool bDoCapture)
{

}


Window::~Window()
{
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef USE_IMGUI

    // imgui event catch
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
    {
        return true;
    }

#endif //USE_IMGUI


    switch (msg)
    {
    case WM_CREATE:
    {
        Window* window = (Window*)((LPCREATESTRUCT)lparam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        window->SetHWND(hwnd);
        //Application::SetHWND(hwnd);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_QUIT:
        PostQuitMessage(0);
        break;
    case WM_SETFOCUS:
    {
        //Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        //window->onFocus();
        break;
    }
    case WM_KILLFOCUS:
    {
        //Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        //window->onKillFocus();
        break;
    }
    case WM_ENTERSIZEMOVE:
    {
        // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
        Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        window->startTimer();
        break;
    }
    case WM_EXITSIZEMOVE:
    {
        // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
        // Here we reset everything based on the new window dimensions.
        Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        window->stopTimer();
        break;
    }
    case WM_ACTIVATEAPP:
        Keyboard::ProcessMessage(msg, wparam, lparam);
        break;

    case WM_KEYDOWN:
        if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
        Keyboard::ProcessMessage(msg, wparam, lparam);
        break;
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard::ProcessMessage(msg, wparam, lparam);
        break;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return 0;
};
