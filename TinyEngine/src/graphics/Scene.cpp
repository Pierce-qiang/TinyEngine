#include "Scene.h"
#include "model/shapes/Sphere.h"
#include "texture/TextureLoader.h"
#include "../platform/window/WindowManager.h"
namespace TEngine {
	Scene::Scene(int sceneID) :
		mCamera(new Camera()),
		mModelRenderer(new ModelRenderer(mCamera)),
		mLightManager(new LightManager())
	{


		Init(sceneID);
	}
	Scene::~Scene()
	{
		delete mCamera;
		delete mModelRenderer;
		delete mLightManager;
		delete mSkybox;
		for (auto item : mModels) delete item;
		mModels.clear();
	}

	void Scene::ProcessInput(float deltaTime)
	{
		// just process the user's input
		mCamera->ProcessInput(deltaTime);
		WindowManager::SetDeltaTime(deltaTime);
	}

	void Scene::AddModelsToRender()
	{
		// add all models to renderer's render queue
		for (auto it : mModels)
			mModelRenderer->SubmitRenderModel(it);

	}
	void Scene::Init(int sceneID)
	{
		// TODO: initialize model to mModels according to sceneID
		Model* cube1 = new Model(Cube());
		mModels.push_back(cube1);
		cube1->SetPosition(glm::vec3(7.0, 3.0f, -5.0f));
		cube1->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));
		//TODO: set material texture
		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;
		cube1->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/texture/wood.png", &srgbTextureSettings));


		Model* cube2 = new Model(Cube());
		cube2->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		cube2->SetScale(glm::vec3(30.0f, 1.0f, 30.0f));
		mModels.push_back(cube2);
		cube2->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/texture/white.png", &srgbTextureSettings));


		Model* cube3 = new Model(Cube());
		cube3->SetPosition(glm::vec3(10.0f, 3.0f, 3.0f));
		cube3->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));
		mModels.push_back(cube3);
		cube3->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/texture/white.png", &srgbTextureSettings));

		Model* sphere1 = new Model(Sphere());
		mModels.push_back(sphere1);
		sphere1->SetPosition(glm::vec3(-10.0, 3.0f, 0.0f));
		sphere1->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/texture/wood.png", &srgbTextureSettings));

#pragma region gunSetting
		//Model* gun = new Model("res/cerberus/meshes/cerberus.obj");
		//mModels.push_back(gun);
		//gun->SetPosition(glm::vec3(2.0f, 0.5f, 0.0f));
		//gun->SetScale(glm::vec3(5.0f, 5.0f, 5.0f));
		//gun->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/cerberus/materials/cerberus_albedo.png", &srgbTextureSettings));
		//gun->GetMesh(0)->GetMaterial()->SetNormalMap(TextureLoader::Load2DTexture("res/cerberus/materials/cerberus_normal.png"));
		//gun->GetMesh(0)->GetMaterial()->SetRoughnessMap(TextureLoader::Load2DTexture("res/cerberus/materials/cerberus_rough.png"));
		//gun->GetMesh(0)->GetMaterial()->SetMetallicMap(TextureLoader::Load2DTexture("res/cerberus/materials/cerberus_metal.png"));
		//gun->GetMesh(0)->GetMaterial()->SetAmbientOcclusionMap(TextureLoader::Load2DTexture("res/cerberus/materials/cerberus_ao.png"));
#pragma endregion
		//skeleton animation
	/*	Model* vampir = new Model("res/vampire/dancing_vampire.dae", true);
		vampir->LoadAnimation("res/vampire/dancing_vampire.dae");
		vampir->SetScale(glm::vec3(0.05f));
		mModels.push_back(vampir);*/

		//Model* sponza = new Model("res/sponza/sponza.obj");
		//Model* gameboy = new Model("res/gameboy/scene.gltf");
		Model* vikingsword = new Model("res/vikingsword/scene.gltf");
		//Model* chess = new Model("res/Chess/scene.gltf");
		//mModels.push_back(gameboy);
		//mModels.push_back(chess);
		mModels.push_back(vikingsword);
		//mModels.push_back(sponza);
		//chess->SetPosition(glm::vec3(5.0, 5.0f, 5.0f));
		//chess->SetScale(glm::vec3(0.035f, 0.035f, 0.035f));
		vikingsword->SetPosition(glm::vec3(5.0, 5.0f, 0.0f));
		vikingsword->SetScale(glm::vec3(0.035f, 0.035f, 0.035f));
		//gameboy->SetPosition(glm::vec3(0.0, 5.0f, 0.0f));
		//gameboy->SetScale(glm::vec3(0.015f, 0.015f, 0.015f));

		//sponza->SetPosition(glm::vec3(0.02f, 0.02f, -50.02f));
		//sponza->SetScale(glm::vec3(0.015f, 0.015f, 0.015f));



		
		//TODO: skybox
		std::vector<std::string> skyboxFilePaths;
		skyboxFilePaths.push_back("res/skybox/right.png");
		skyboxFilePaths.push_back("res/skybox/left.png");
		skyboxFilePaths.push_back("res/skybox/top.png");
		skyboxFilePaths.push_back("res/skybox/bottom.png");
		skyboxFilePaths.push_back("res/skybox/back.png");
		skyboxFilePaths.push_back("res/skybox/front.png");
		mSkybox = new Skybox(skyboxFilePaths);
	}
}