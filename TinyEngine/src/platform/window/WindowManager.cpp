#include "WindowManager.h"
#include <iostream>
#include "../../macros.h"
#include "InputManager.h"

namespace TEngine {
	//static member data must init
	WindowManager* WindowManager::sInstance = nullptr;
	float WindowManager::deltaTime = 0.0f;
	WindowManager::WindowManager(){}

	WindowManager::~WindowManager(){}

	//register callback functions first
	void ErrorCallback(int error, const char* description);
	void WindowResizeCallback(GLFWwindow* window, int width, int height);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void CharCallback(GLFWwindow* window, unsigned int c);
	// singleton pattern
	WindowManager* WindowManager::Instance()
	{
		if (sInstance == nullptr)
			sInstance = new WindowManager();

		return sInstance;
	}

	bool WindowManager::Init(const char* title, int width, int height)
	{
		mTitle = title;
		mWidth = width;
		mHeight = height;
		mHideCursor = false;

		// glfw: initialize and configure
		if (!glfwInit()) 
		{
			std::cout << "GLFW Failed To Initialize" << std::endl;
			return false;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		//glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

		// create the window
		mWindow = glfwCreateWindow(mWidth, mHeight, mTitle, NULL, NULL);

		if (!mWindow) 
		{
			std::cout << "GLFW Window Created Unsuccessfully" << std::endl;
			return false;
		}

		// setup the mouse settings
		if (mHideCursor)
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double currentMouseX, currentMouseY;
		glfwGetCursorPos(mWindow, &currentMouseX, &currentMouseY);
		InputManager::Instance()->SetMousePos(currentMouseX, currentMouseY);

		// set up contexts and callbacks
		glfwMakeContextCurrent(mWindow);
		glfwSetWindowUserPointer(mWindow, this);
		glfwSetErrorCallback(ErrorCallback);
		glfwSetWindowSizeCallback(mWindow, WindowResizeCallback);
		glfwSetKeyCallback(mWindow, KeyCallback);
		glfwSetMouseButtonCallback(mWindow, MouseButtonCallback);
		glfwSetCursorPosCallback(mWindow, CursorPositionCallback);
		glfwSetScrollCallback(mWindow, ScrollCallback);
		glfwSetCharCallback(mWindow, CharCallback);

		// v-sync settings
		glfwSwapInterval(V_SYNC);
		
		// load all opengl function pointers with glad
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}
		std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

		// setup default openGL viewport
		glViewport(0, 0, mWidth, mHeight);

		return true;
	}

	bool WindowManager::IsTerminated()
	{
		return glfwWindowShouldClose(mWindow);
	}

	void WindowManager::Update()
	{
		InputManager::Instance()->ResetDeltas();

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
	void WindowManager::ReleaseResource() {
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}


	void WindowManager::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	// callback function definitions
	// -----------------------------
	void WindowManager::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ErrorCallback(int error, const char* description)
	{
		std::cout << "Error:" << std::endl << description << std::endl;
	}

	void WindowResizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowManager* wm = (WindowManager*)glfwGetWindowUserPointer(window);
		wm->mWidth = width;
		wm->mHeight = height;

		glViewport(0, 0, width, height);
	}

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WindowManager* wm = (WindowManager*)glfwGetWindowUserPointer(window);
		InputManager::Instance()->KeyCallback(key, scancode, action, mods);

		if (key == GLFW_KEY_H && action == GLFW_RELEASE)
		{
			wm->mHideCursor = !wm->mHideCursor;
			GLenum cursorOption = wm->mHideCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
			glfwSetInputMode(wm->mWindow, GLFW_CURSOR, cursorOption);
		}


		if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
			glfwSetWindowShouldClose(window, true);
	}

	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		InputManager::Instance()->MouseButtonCallback(button, action, mods);
	}

	void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
	{
		InputManager::Instance()->CursorPositionCallback(xpos, ypos);
	}

	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		InputManager::Instance()->ScrollCallback(xoffset, yoffset);
	}

	void CharCallback(GLFWwindow* window, unsigned int c)
	{
	}

}