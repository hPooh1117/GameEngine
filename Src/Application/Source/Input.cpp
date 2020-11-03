#include "Input.h"


using DirectX::Keyboard;

//-------------------------------------------------------------------------------------------------------
// SETTING KEYS
//-------------------------------------------------------------------------------------------------------
const Input::KeyMap Input::KEYMAPS = []()
{
	KeyMap m;

	m["F1"] = Keyboard::F1;  m["F2"] = Keyboard::F2;    m["F3"] = Keyboard::F3;    m["F4"] = Keyboard::F4;
	m["F5"] = Keyboard::F5;  m["F6"] = Keyboard::F6;    m["F7"] = Keyboard::F7;    m["F8"] = Keyboard::F8;
	m["F9"] = Keyboard::F9;  m["F10"] = Keyboard::F10;  m["F11"] = Keyboard::F11;  m["F12"] = Keyboard::F12;

	m["0"] = Keyboard::D0;   m["1"] = Keyboard::D1;     m["2"] = Keyboard::D2;     m["3"] = Keyboard::D3;
	m["4"] = Keyboard::D4;   m["5"] = Keyboard::D5;     m["6"] = Keyboard::D6;     m["7"] = Keyboard::D7;
	m["8"] = Keyboard::D8;   m["9"] = Keyboard::D9;     

	m["Q"] = Keyboard::Q;    m["W"] = Keyboard::W;      m["E"] = Keyboard::E;      m["R"] = Keyboard::R;
	m["A"] = Keyboard::A;    m["S"] = Keyboard::S;      m["D"] = Keyboard::D;      m["F"] = Keyboard::F;
	m["Z"] = Keyboard::Z;    m["X"] = Keyboard::X;      m["C"] = Keyboard::C;      m["V"] = Keyboard::V;
	m["B"] = Keyboard::B;

	m["Left"] = Keyboard::Left;
	m["Right"] = Keyboard::Right;
	m["Up"] = Keyboard::Up;
	m["Down"] = Keyboard::Down;

	m["LeftShift"] = Keyboard::LeftShift;
	m["LeftAlt"] = Keyboard::LeftAlt;
	m["LeftCtrl"] = Keyboard::LeftControl;
	m["Space"] = Keyboard::Space;

	m["Enter"] = Keyboard::Keyboard::Enter;
	m["Back"] = Keyboard::Keyboard::Back;
	m["Esc"] = Keyboard::Escape;


	return std::move(m);
}();

//-------------------------------------------------------------------------------------------------------
// SETTING GAMEPAD
//-------------------------------------------------------------------------------------------------------
const Input::PadMap Input::PADMAPS = []()
{
	PadMap m;
	m["Up"] = kGamePad::kUP;  m["Down"] = kGamePad::kDOWN;  m["Left"] = kGamePad::kLEFT;  m["Right"] = kGamePad::kRIGHT;
	m["A"] = kGamePad::kA;    m["B"] = kGamePad::kB;        m["X"] = kGamePad::kX;        m["Y"] = kGamePad::kY;
	m["L1"] = kGamePad::kLSHOULDER;  
	m["R1"] = kGamePad::kRSHOULDER;

	m["Start"] = kGamePad::kSTART;
	m["Back"] = kGamePad::kBACK;

	return std::move(m);
}();



//-------------------------------------------------------------------------------------------------------
// CON/DES
//-------------------------------------------------------------------------------------------------------
Input::Input()
{
	m_pKeyBoard = std::make_unique<Keyboard>();
}
Input::~Input()
{
}

//-----------------------------------------------------------------------------
// UPDATE PER FRAME
//-----------------------------------------------------------------------------
void Input::HandleInput(HWND hwnd)
{
	// キー入力更新
	memcpy(mPrevKeys, mKeys, sizeof(bool) * KEY_COUNT);
	ZeroMemory(mKeys, ARRAYSIZE(mKeys));

	Keyboard::State keyState = m_pKeyBoard->GetState();

	for (auto& key : KEYMAPS)
	{
		if (keyState.IsKeyDown((Keyboard::Keys)key.second))
		{
			mKeys[key.second] = true;
		}
	}

	// ゲームパッド更新
	UpdateGamePad();

	// マウス位置更新
	//UpdateMouse(hwnd);
	Vector2 prevMousePos = mMousePos;
	POINT mousePos;
	::GetCursorPos(&mousePos);
	::ScreenToClient(hwnd, &mousePos);
	mMousePos = Vector2(mousePos.x, mousePos.y);

	// マウス移動量更新
	mMouseDelta = mMousePos - prevMousePos;

}

void Input::UpdateGamePad()
{
	DWORD dwResult;
	memcpy(&mPrevState, &mCurrentState, sizeof(XINPUT_STATE));

	ZeroMemory(&mCurrentState, sizeof(XINPUT_STATE));

	dwResult = XInputGetState(0, &mCurrentState);

	if (dwResult == ERROR_SUCCESS)
	{
		m_bConnected = true;
	}
	else
	{
		m_bConnected = false;
	}
}

//void Input::UpdateMouse(HWND hwnd)
//{
//
//	if (m_bMouseCaptured)
//	{
//		RECT rcClip; GetWindowRect(hwnd, &rcClip);
//
//		constexpr int PX_OFFSET = 15;
//		constexpr int PX_WND_TITLE_OFFSET = 30;
//		rcClip.left   += PX_OFFSET;
//		rcClip.right  -= PX_OFFSET;
//		rcClip.top    += PX_OFFSET + PX_WND_TITLE_OFFSET;
//		rcClip.bottom -= PX_OFFSET;
//
//		while (ShowCursor(FALSE) >= 0);
//		ClipCursor(&rcClip);
//		GetCursorPos(&mousePos);
//		SetForegroundWindow(hwnd);
//		SetFocus(hwnd);
//	}
//	else
//	{
//		ClipCursor(nullptr);
//		SetCursorPos(mousePos.x, mousePos.y);
//		while (ShowCursor(TRUE) <= 0);
//		SetForegroundWindow(NULL);
//		
//	}
//
//}


//-----------------------------------------------------------------------------
// KEYBOARD INPUT
//-----------------------------------------------------------------------------
bool Input::OnKeyDown(const char* key) const
{
	const unsigned int code = KEYMAPS.find(key)->second;
	return mKeys[code];
}

bool Input::OnKeyUp(const char* key) const
{
	const unsigned int code = KEYMAPS.find(key)->second;
	return !mKeys[code];
}

bool Input::OnKeyTrigger(const char* key) const
{
	const unsigned int code = KEYMAPS.find(key)->second;
	return mKeys[code] && (!mPrevKeys[code]);
}

bool Input::OnKeyRelease(const char* key) const
{
	const unsigned int code = KEYMAPS.find(key)->second;
	return (!mKeys[code]) && mPrevKeys[code];
}

//-----------------------------------------------------------------------------
// MOUSE INPUT
//-----------------------------------------------------------------------------
bool Input::OnMouseMove() const
{
	return !(mMouseDelta.x == 0.0f && mMouseDelta.y == 0.0f);
}

const Vector2 &Input::GetMousePos() const
{
	return mMousePos;
}

const Vector2 &Input::GetMouseDelta() const
{ 
	return mMouseDelta;
}

bool Input::GetIsMouseWheelUp() const
{
	return mMouseWheelState == MouseState::EWheelUp;
}

bool Input::GetIsMouseWheelDown() const
{
	return mMouseWheelState == MouseState::EWheelDown;
}

void Input::MakeMouseCaptured()
{
	m_bMouseCaptured = true;
}

void Input::MakeMouseReleased()
{
	m_bMouseCaptured = false;
}



//-----------------------------------------------------------------------------
// GAMEPAD INPUT
//-----------------------------------------------------------------------------
bool Input::IsConnected(int playerNo) const
{
	return m_bConnected;
}

//
// BUTTON
//
bool Input::OnButtonDown(const char* key) const
{
	if (!m_bConnected) return false;

	const WORD padCode = PADMAPS.find(key)->second;
	return (mCurrentState.Gamepad.wButtons & padCode) != 0;
}

bool Input::OnButtonUp(const char* key) const 
{
	if (!m_bConnected) return false;

	const WORD padCode = PADMAPS.find(key)->second;
	return (mCurrentState.Gamepad.wButtons & padCode) == 0;
}

bool Input::OnButtonTrigger(const char* key) const
{
	if (!m_bConnected) return false;

	const WORD padCode = PADMAPS.find(key)->second;
	return (mCurrentState.Gamepad.wButtons & padCode) != 0 &&
		(mPrevState.Gamepad.wButtons & padCode) == 0;
}

bool Input::OnButtonRelease(const char* key) const
{
	if (!m_bConnected) return false;

	const WORD padCode = PADMAPS.find(key)->second;
	return (mCurrentState.Gamepad.wButtons & padCode) == 0 &&
		(mPrevState.Gamepad.wButtons & padCode) != 0;
}

//
// TRIGGER
//
bool Input::OnLeftTriggerDown() const
{
	if (!m_bConnected) return false;

	return static_cast<float>(mCurrentState.Gamepad.bLeftTrigger) / 255.0f > 0.0f;
}

bool Input::OnRightTriggerDown() const
{
	if (!m_bConnected) return false;

	return static_cast<float>(mCurrentState.Gamepad.bRightTrigger) / 255.0f > 0.0f;
}

float Input::GetLeftTriggerValue() const
{
	if (!m_bConnected) return 0.0f;

	return static_cast<float>(mCurrentState.Gamepad.bLeftTrigger) / 255.0f;
}

float Input::GetRightTriggerValue() const
{
	if (!m_bConnected) return 0.0f;

	return static_cast<float>(mCurrentState.Gamepad.bRightTrigger);
}

//
// THUMB STICK
//

float Input::GetThumbLXValue() const
{
	if (!m_bConnected) return 0.0f;

	return static_cast<float>(mCurrentState.Gamepad.sThumbLX) / 32767.0f;
}

float Input::GetThumbLYValue() const
{
	if (!m_bConnected) return 0.0f;

	return static_cast<float>(mCurrentState.Gamepad.sThumbLY) / 32767.0f;
}

float Input::GetThumbRXValue() const
{
	if (!m_bConnected) return 0.0f;

	return static_cast<float>(mCurrentState.Gamepad.sThumbRX) / 32767.0f;
}

float Input::GetThumbRYValue() const
{
	if (!m_bConnected) return 0.0f;

	return static_cast<float>(mCurrentState.Gamepad.sThumbRY) / 32767.0f;
}

void Input::SetMouseWheelState(int state)
{
	mMouseWheelState = state;
}



//-----------------------------------------------------------------------------
// HELPER
//-----------------------------------------------------------------------------
//Input* Input::Get()
//{
//	static Input instance;
//	return &instance;
//}
//
