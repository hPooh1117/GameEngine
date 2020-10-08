#pragma once
#define USE_IMGUI

#ifdef USE_IMGUI

#include <windows.h>

#include "./3rdParty/imgui/imgui.h"
#include "./3rdParty/imgui/imgui_internal.h"
#include "./3rdParty/imgui/imgui_impl_dx11.h"
#include "./3rdParty/imgui/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif