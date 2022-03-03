#pragma once


namespace TEngine {
	class InputManager
	{
	public:
		~InputManager();
		static InputManager* Instance();
		
		bool IsMouseButtonPressed(unsigned int code);
		bool IsKeyPressed(unsigned int keycode);
		void ResetDeltas();
		//callback func
		void KeyCallback(int key, int scancode, int action, int mods);
		void MouseButtonCallback(int button, int action, int mods);
		void CursorPositionCallback(double xpos, double ypos);
		void ScrollCallback(double xoffset, double yoffset);


		// Getters and setters
		inline double GetMouseX() { return mMouseX; }
		inline double GetMouseY() { return mMouseY; }
		inline double GetMouseXDelta() { return mMouseXDelta; }
		inline double GetMouseYDelta() { return mMouseYDelta; }
		inline double GetScrollXDelta() { return mScrollXDelta; }
		inline double GetScrollYDelta() { return mScrollYDelta; }
		inline void	  GetMousePosition(double& x, double& y) { x = mMouseX; y = mMouseY; }

		inline void SetMouseX(double value) { mMouseX = value; }
		inline void SetMouseY(double value) { mMouseY = value; }
		inline void SetMousePos(double xPos, double yPos) { mMouseX = xPos, mMouseY = yPos; }


	private:
		static InputManager* sInstance;
		InputManager();


	private:
		static const int MAX_KEY_NUM = 1024;
		static const int MAX_BUTTON_NUM = 64;

		// store buttons and keys info
		bool mButtons[MAX_BUTTON_NUM];
		bool mKeys[MAX_KEY_NUM];

		// store mouse related info
		double mMouseX, mMouseY, mMouseXDelta, mMouseYDelta;
		double mScrollXDelta, mScrollYDelta;
	};
}


