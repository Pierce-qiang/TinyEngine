#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace TEngine {
	class WindowManager
	{
	public:
		static WindowManager* Instance();
		~WindowManager();

		bool Init(const char* title, int width, int height);

		int GetWidth() { return mWidth; }
		int GetHeight() { return mHeight; }
		bool IsHideCursor() { return mHideCursor; }
		GLFWwindow* GetWindow() { return mWindow; }

		bool IsTerminated();
		void Update();
		void ReleaseResource();
		static void Clear();	// clear depth, color and stencil buffer
		static void Bind();		// bind the default framebuffer
		static void SetDeltaTime(float delTime) { deltaTime = delTime; }
		static float GetDeltaTime() { return deltaTime; }
	private:
		WindowManager();
		static WindowManager* sInstance;

		// callback functions which cannot be member functions
		// but we need to visit private data, friend is needed 
		friend void ErrorCallback(int error, const char* description);
		friend void WindowResizeCallback(GLFWwindow* window, int width, int height);
		friend void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		friend void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		friend void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
		friend void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		friend void CharCallback(GLFWwindow* window, unsigned int c);

	private:
		const char* mTitle;
		int mWidth, mHeight;
		bool mHideCursor;
		
		static float deltaTime;

		GLFWwindow* mWindow;
	};
}



