#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "platform/window/InputManager.h"
#include "platform/window/WindowManager.h"
#include "graphics/texture/TextureLoader.h"
#include "graphics/Skybox.h"
using namespace TEngine;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	InputManager::Instance();
	if (!WindowManager::Instance()->Init("SRenderEngine", SCR_WIDTH, SCR_HEIGHT))
	{
		std::cout << "Could not initialize window class!\n";
		glfwTerminate();
	}
    const std::vector<std::string> skyboxPath = {
        "res/skybox/right.png",
        "res/skybox/left.png",
        "res/skybox/top.png",
        "res/skybox/bottom.png",
        "res/skybox/back.png",
        "res/skybox/front.png"
    };

    Skybox skybox(skyboxPath);
    Camera camera;

    TextureLoader::InitDefaultTextures();
    // render loop
    // -----------
    while (!WindowManager::Instance()->IsTerminated())
    {
        // input
        // -----
        camera.ProcessInput(0.1f);
        // render
        // ------
		WindowManager::Instance()->Clear();
		WindowManager::Instance()->Bind();
        skybox.Render(&camera);

		//renderPassManager.SaverRenderFrame("./test.png");
		// swap buffers and poll IO events
		// -------------------------------
        WindowManager::Instance()->Update();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}