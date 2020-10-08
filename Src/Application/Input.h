#pragma once
#include <memory>
#include <unordered_map>
#include <Keyboard.h>
#include <GamePad.h>
#include <Windows.h>
#include "./Utilities/Vector.h"
#include <Xinput.h>

#pragma comment(lib, "xinput.lib")


class Input final
{
private:
	using KeyMap = std::unordered_map<const char*, unsigned int>;
	static const KeyMap KEYMAPS;
	using PadMap = std::unordered_map<const char*, WORD>;
	static const PadMap PADMAPS;

	static constexpr unsigned int KEY_COUNT = 256;

	enum kGamePad : WORD
	{
		kUP = 0x0001,
		kDOWN = 0x0002,
		kLEFT = 0x0004,
		kRIGHT = 0x0008,
		kA = 0x1000,
		kB = 0x2000,
		kX = 0x4000,
		kY = 0x8000,
		kLSTICK = 0x0040,
		kRSTICK = 0x0080,
		kLSHOULDER = 0x0100,
		kRSHOULDER = 0x0200,
		kSTART = 0x0010,
		kBACK = 0x0020,

		kNUM_MAX,
	};

private:
	std::unique_ptr<DirectX::Keyboard>  m_pKeyBoard;
	std::unique_ptr<DirectX::GamePad>   m_pGamePad;

private:
	bool                                mKeys[KEY_COUNT];
	bool                                mPrevKeys[KEY_COUNT];
				                        
	bool                                m_bConnected;
	XINPUT_STATE                        mPrevState;
	XINPUT_STATE                        mCurrentState;
				                        
	bool                                m_bMouseCaptured;
	Vector2                             mMousePos;
	Vector2                             mMouseDelta;


public:
// UPDATE PER FRAME
	void HandleInput(HWND hwnd);
private:
	void UpdateGamePad();
	void UpdateMouse(HWND hwnd);

public:
// KEYBOARD INPUT
	bool    OnKeyDown(const char* key) const;
	bool    OnKeyUp(const char* key) const;
		    
	bool    OnKeyTrigger(const char* key) const;
	bool    OnKeyRelease(const char* key) const;

	bool    OnMouseMove() const;
	Vector2 GetMousePos() const;
	Vector2 GetMouseDelta() const;
	void    MakeMouseCaptured();
	void    MakeMouseReleased();

// GAMEPAD INPUT
	bool IsConnected(int playerNo = 0) const;
	bool OnButtonDown(const char* key) const;
	bool OnButtonUp(const char* key) const;
	bool OnButtonTrigger(const char* key) const;
	bool OnButtonRelease(const char* key) const;

	bool OnLeftTriggerDown() const;
	bool OnRightTriggerDown() const;
	float GetLeftTriggerValue() const;
	float GetRightTriggerValue() const;
	float GetThumbLXValue() const;
	float GetThumbLYValue() const;
	float GetThumbRXValue() const;
	float GetThumbRYValue() const;

// STATIC INTERFACE
	static Input* Get();
	~Input();

private:
	Input();
	Input(const Input& other) = delete;
	Input& operator=(const Input& other) = delete;
};

#define InputPtr  Input::Get()