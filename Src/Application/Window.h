#pragma once
#include <Windows.h>
#include "./Renderer/high_resolution_timer.h"
#include <sstream>

class Window
{
private:
    bool m_bMouseCaptured;
    bool m_bAppWantsExit;

protected:
    HWND mHwnd;
    high_resolution_timer m_high_resolution_timer;


public:
    Window();
    virtual bool Initialize();
    void ShowAndUpdateWindow();
    bool Run();
    void SetHWND(HWND hwnd);
    void calculateFrameStats();

    void startTimer();
    void stopTimer();


    virtual void Update(float elapsed_time) = 0;
    virtual void Render(float elapsed_time) = 0;
    
    void CaptureMouse(bool bDoCapture);
    //virtual void onFocus() = 0;
    //virtual void onKillFocus() = 0;
    virtual void OnSize() = 0;

    inline RECT GetClientWindowRect()
    {
        RECT rect = {};
        ::GetClientRect(mHwnd, &rect);
        return rect;
    }

    virtual ~Window();
};