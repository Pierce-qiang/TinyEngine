#include <iostream>
#include <unordered_map>
#include <ctime>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "platform/window/WindowManager.h"
#include "platform/window/InputManager.h"
#include "graphics/Scene.h"
#include "graphics/model/Model.h"
#include "graphics/renderer/RenderPassManager.h"
#include "graphics/texture/TextureLoader.h"
#include "graphics/Shader.h"
#include "graphics/GuiManager.h"
using namespace TEngine;
//unsigned int LoadTexture(const char* path);
//void renderScene(Shader& shader);
//void renderCube();

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// meshes
unsigned int planeVAO, planeVBO;




int main()
{
	// Init
	InputManager::Instance();
	if (!WindowManager::Instance()->Init("TinyEngine", 1600, 900))
	{
		std::cout << "Could not initialize window class!\n";
		glfwTerminate();
	}

	TextureLoader::InitDefaultTextures();
	Scene scene(0);
	RenderPassManager renderPassManager(&scene);
	GuiManager guiManager(&scene, &renderPassManager);


	while (!WindowManager::Instance()->IsTerminated())
	{
		// calculate per frame time
		// ------------------------
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// process input events
		// --------------------
		scene.ProcessInput(deltaTime);
		
		// render
		// ------
		// TODO: If set glclearcolor to some "actual color", it will have some bugs in deferred renering because of the color buffer value
		// So I leave it to black here, but it's more reasonable to use a stencil test to do deferred rendering.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearStencil(0);
		WindowManager::Instance()->Clear();
		WindowManager::Instance()->Bind();
		renderPassManager.Render();
		guiManager.Draw();
		//renderPassManager.SaverRenderFrame("./test.png");
		// swap buffers and poll IO events
		// -------------------------------
		WindowManager::Instance()->Update();
		//std::cout << scene.GetCamera()->GetPosition()[0] <<" "<< scene.GetCamera()->GetPosition()[1]
		//	<< " " << scene.GetCamera()->GetPosition()[2] << "\n";
	}
	TextureLoader::DestroyCachedTexture();
	guiManager.EndGui();
	WindowManager::Instance()->ReleaseResource();
	return 0;
}































//int main()
//{
//	if (!WindowManager::Instance()->Init("SRenderEngine", 800, 600))
//	{
//		std::cout<< "Could not initialize window class!\n";
//		glfwTerminate();
//	}
//
//	Scene scene;
//
//	//Model model("./res/Sponza/sponza.obj");
//	
//
//	// configure global opengl state
//	// -----------------------------
//	glEnable(GL_DEPTH_TEST);
//	glfwWindowHint(GLFW_SAMPLES, 16);
//	glEnable(GL_MULTISAMPLE);
//	// set depth function to less than AND equal for skybox depth trick.
//	glDepthFunc(GL_LEQUAL);
//	// enable seamless cubemap sampling for lower mip levels in the pre-filter map.
//	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
//
//	// compile shader
//	// --------------
//	std::unordered_map<std::string, std::string> smShaderPaths;
//	smShaderPaths.insert({ "vertex","res/shader/shadowmap_generation.vert" });
//	smShaderPaths.insert({ "fragment","res/shader/shadowmap_generation.frag" });
//	Shader shadowmapShader(smShaderPaths);
//
//	std::unordered_map<std::string, std::string> blinnShaderPaths;
//	blinnShaderPaths.insert({ "vertex","res/shader/blinnphong.vert" });
//	blinnShaderPaths.insert({ "fragment","res/shader/blinnphong.frag" });
//	Shader blinnShader(blinnShaderPaths);
//
//	// set up vertex data (and buffer(s)) and configure vertex attributes
//	// ------------------------------------------------------------------
//	float planeVertices[] = 
//	{
//		// positions            // normals         // texcoords
//		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
//		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
//		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
//
//		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
//		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
//		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
//	};
//
//	// create plane VAO
//	// ----------------
//	glGenVertexArrays(1, &planeVAO);
//	glGenBuffers(1, &planeVBO);
//	glBindVertexArray(planeVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//	glBindVertexArray(0);
//
//	// load texture
//	// ------------
//	unsigned int floorTexture = LoadTexture("res/texture/wood.png");
//
//	// configure shadow map FBO
//	// ------------------------
//	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
//	unsigned int shadowmapFBO;
//	glGenFramebuffers(1, &shadowmapFBO);
//	// create shadow map
//	unsigned int shadowmap;
//	glGenTextures(1, &shadowmap);
//	glBindTexture(GL_TEXTURE_2D, shadowmap);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	float borderColor[] = { 1.0f, 1.0f,1.0f,1.0f };
//	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
//	// attach depth texture
//	glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmap, 0);
//	glDrawBuffer(GL_NONE);
//	glReadBuffer(GL_NONE);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//
//	// shader configuration
//	// --------------------
//	blinnShader.Bind();
//	blinnShader.SetUniform("floorTexture", 0);
//	blinnShader.SetUniform("shadowMap", 1);
//
//	// lighting info
//	// -------------
//	glm::vec3 lightPos(5.0f, 5.5f,5.0f);
//
//	while (!WindowManager::Instance()->IsTerminated())
//	{
//		// calculate per frame time
//		// ------------------------
//		float currentFrame = (float)glfwGetTime();
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//		// process input events
//		// --------------------
//		scene.Update(deltaTime);
//
//		// render
//		// ------
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// 1. render depth of scene to texture (from light's perspective)
//		// --------------------------------------------------------------
//		glm::mat4 lightProjection, lightView;
//		glm::mat4 lightSpaceMatrix;
//		float near_plane = 0.0f, far_plane = 25.5f;
//		lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
//		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
//		lightSpaceMatrix = lightProjection * lightView;
//		// render scene from light's point of view
//		shadowmapShader.Bind();
//		shadowmapShader.SetUniform("lightSpaceMatrix", lightSpaceMatrix);
//
//		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
//		glClear(GL_DEPTH_BUFFER_BIT);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, floorTexture);
//		renderScene(shadowmapShader);
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//		// reset viewport
//		glViewport(0, 0, 800, 600);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//		// 2. render scene as normal using the generated depth/shadow map  
//		// --------------------------------------------------------------
//		glViewport(0, 0, WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		blinnShader.Bind();
//		glm::mat4 projection = scene.GetCamera()->GetProjectionMatrix();
//		glm::mat4 view = scene.GetCamera()->GetViewMatrix();
//		blinnShader.SetUniform("projection", projection);
//		blinnShader.SetUniform("view", view);
//		// set light uniforms
//		blinnShader.SetUniform("viewPos", scene.GetCamera()->GetPosition());
//		blinnShader.SetUniform("lightPos", lightPos);
//		blinnShader.SetUniform("lightSpaceMatrix", lightSpaceMatrix);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, floorTexture);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, shadowmap);
//		renderScene(blinnShader);
//
//
//		// swap buffers and poll IO events
//		// -------------------------------
//		WindowManager::Instance()->Update();
//	}
//
//	glfwTerminate();
//	return 0;
//}

//
//unsigned int LoadTexture(const char* path)
//{
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//
//	int width, height, nComponents;
//	unsigned char* data = stbi_load(path, &width, &height, &nComponents, 0);
//	if (data)
//	{
//		GLenum format;
//		if (nComponents == 1)
//			format = GL_RED;
//		else if (nComponents == 3)
//		{
//			format = GL_RGB;
//			std::cout << "GL_RGB" << std::endl;
//
//		}
//		else if (nComponents == 4)
//		{
//			format = GL_RGBA;
//			std::cout << "GL_RGBA" << std::endl;
//		}
//
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		// texture settings
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		stbi_image_free(data);
//	}
//	else
//	{
//		std::cout << "Texture failed to load at path: " << path << std::endl;
//		stbi_image_free(data);
//	}
//
//	return textureID;
//}
//
//
//// renders the 3D scene
//// --------------------
//void renderScene(Shader& shader)
//{
//	// floor
//	glm::mat4 model = glm::mat4(1.0f);
//	shader.SetUniform("model", model);
//	glBindVertexArray(planeVAO);
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//	// cubes
//	model = glm::mat4(1.0f);
//	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
//	model = glm::scale(model, glm::vec3(0.5f));
//	shader.SetUniform("model", model);
//	renderCube();
//	model = glm::mat4(1.0f);
//	model = glm::translate(model, glm::vec3(4.0f, 0.0f, 1.0));
//	model = glm::scale(model, glm::vec3(0.5f));
//	shader.SetUniform("model", model);
//	renderCube();
//	model = glm::mat4(1.0f);
//	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
//	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
//	model = glm::scale(model, glm::vec3(0.5f));
//	shader.SetUniform("model", model);
//	renderCube();
//}
//
//
//// renderCube() renders a 1x1 3D cube in NDC.
//// -------------------------------------------------
//unsigned int cubeVAO = 0;
//unsigned int cubeVBO = 0;
//void renderCube()
//{
//	// initialize (if necessary)
//	if (cubeVAO == 0)
//	{
//		float vertices[] = {
//			// back face
//			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
//			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
//			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
//			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
//			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
//			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
//			// front face
//			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
//			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
//			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
//			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
//			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
//			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
//			// left face
//			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
//			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
//			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
//			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
//			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
//			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
//			// right face
//			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
//			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
//			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
//			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
//			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
//			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
//			// bottom face
//			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
//			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
//			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
//			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
//			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
//			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
//			// top face
//			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
//			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
//			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
//			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
//			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
//			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
//		};
//		glGenVertexArrays(1, &cubeVAO);
//		glGenBuffers(1, &cubeVBO);
//		// fill buffer
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//		// link vertex attributes
//		glBindVertexArray(cubeVAO);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//		glEnableVertexAttribArray(2);
//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindVertexArray(0);
//	}
//	// render Cube
//	glBindVertexArray(cubeVAO);
//	glDrawArrays(GL_TRIANGLES, 0, 36);
//	glBindVertexArray(0);
//}