#include "InputManager.h"
#include "GLFW/glfw3.h"
#include<iostream>

namespace TEngine {
	//static member data must init
	InputManager* InputManager::sInstance = nullptr;

	InputManager::InputManager():mScrollXDelta(0.0),mScrollYDelta(0.0),
		mMouseX(0.0),mMouseY(0.0),mMouseXDelta(0.0),mMouseYDelta(0.0)
	{
		memset(mKeys, 0, sizeof(bool) * MAX_KEY_NUM);
		memset(mButtons, 0, sizeof(bool) * MAX_BUTTON_NUM);
	}

	InputManager::~InputManager()
	{
	}

	InputManager* InputManager::Instance()
	{
		if (sInstance == nullptr)
			sInstance = new InputManager();

		return sInstance;
	}

	bool InputManager::IsMouseButtonPressed(unsigned int code)
	{
		return mButtons[code];
	}

	bool InputManager::IsKeyPressed(unsigned int keycode)
	{
		return mKeys[keycode];
	}

	void InputManager::ResetDeltas()
	{
		mMouseXDelta = mMouseYDelta = 0;
		mScrollXDelta = mScrollYDelta = 0;
	}

	void InputManager::KeyCallback(int key, int scancode, int action, int mods)
	{
		mKeys[key] = (action != GLFW_RELEASE);
	}

	void InputManager::MouseButtonCallback(int button, int action, int mods)
	{
		mButtons[button] = (action != GLFW_RELEASE);
	}

	void InputManager::CursorPositionCallback(double xpos, double ypos)
	{
		mMouseXDelta = xpos - mMouseX;
		mMouseYDelta = ypos - mMouseY;
		mMouseX = xpos;
		mMouseY = ypos;
	}

	void InputManager::ScrollCallback(double xoffset, double yoffset)
	{
		mScrollXDelta = xoffset;
		mScrollYDelta = yoffset;
	}
}