#include "Scene.h"
#include "model/shapes/Sphere.h"
#include "texture/TextureLoader.h"
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
		// just process the user's input now
		mCamera->ProcessInput(deltaTime);
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
		Model* cube2 = new Model(Cube());
		Model* cube3 = new Model(Cube());
		Model* sphere1 = new Model(Sphere());
		//Model* gun = new Model("res/cerberus/meshes/cerberus.obj");
		//Model* sponza = new Model("res/sponza/sponza.obj");
		//Model* gameboy = new Model("res/gameboy/scene.gltf");
		Model* vikingsword = new Model("res/vikingsword/scene.gltf");
		//Model* chess = new Model("res/Chess/scene.gltf");

		//mModels.push_back(gameboy);
		//mModels.push_back(chess);
		mModels.push_back(vikingsword);
		mModels.push_back(sphere1);
		mModels.push_back(cube1);
		mModels.push_back(cube2);
		mModels.push_back(cube3);
		//mModels.push_back(gun);
		//mModels.push_back(sponza);
		//set pos
		
		//Model* sphere1 = new Model(Sphere());
		//Model* sphere2 = new Model(Sphere());
		//mModels.push_back(sphere1);
		//mModels.push_back(sphere2);
		//chess->SetPosition(glm::vec3(5.0, 5.0f, 5.0f));
		//chess->SetScale(glm::vec3(0.035f, 0.035f, 0.035f));
		vikingsword->SetPosition(glm::vec3(5.0, 5.0f, 0.0f));
		vikingsword->SetScale(glm::vec3(0.035f, 0.035f, 0.035f));
		//gameboy->SetPosition(glm::vec3(0.0, 5.0f, 0.0f));
		//gameboy->SetScale(glm::vec3(0.015f, 0.015f, 0.015f));
		sphere1->SetPosition(glm::vec3(0.0, 3.0f, 0.0f));
		cube1->SetPosition(glm::vec3(7.0, 3.0f, -49.0f));
		//cube1->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));
		cube2->SetPosition(glm::vec3(0.0f, -2.0f, 0.0f));
		cube2->SetScale(glm::vec3(30.0f, 1.0f, 30.0f));
		cube3->SetPosition(glm::vec3(1.0f, 3.0f, 3.0f));
		cube3->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));
		//gun->SetPosition(glm::vec3(2.0f, 0.5f, 0.0f));
		//gun->SetScale(glm::vec3(5.0f, 5.0f, 5.0f));
		//sponza->SetPosition(glm::vec3(0.02f, 0.02f, -50.02f));
		//sponza->SetScale(glm::vec3(0.015f, 0.015f, 0.015f));

		//TODO: set material texture
		TextureSettings srgbTextureSettings;
		srgbTextureSettings.IsSRGB = true;
		//dragon->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/texture/wood.png", &srgbTextureSettings));
		sphere1->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/texture/wood.png", &srgbTextureSettings));
		cube1->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/texture/wood.png", &srgbTextureSettings));
		cube2->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/texture/white.png", &srgbTextureSettings));
		cube3->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/texture/white.png", &srgbTextureSettings));
		/*gun->GetMesh(0)->GetMaterial()->SetAlbedoMap(TextureLoader::Load2DTexture("res/cerberus/materials/cerberus_albedo.png", &srgbTextureSettings));
		gun->GetMesh(0)->GetMaterial()->SetNormalMap(TextureLoader::Load2DTexture("res/cerberus/materials/cerberus_normal.png"));
		gun->GetMesh(0)->GetMaterial()->SetRoughnessMap(TextureLoader::Load2DTexture("res/cerberus/materials/cerberus_rough.png"));
		gun->GetMesh(0)->GetMaterial()->SetMetallicMap(TextureLoader::Load2DTexture("res/cerberus/materials/cerberus_metal.png"));
		gun->GetMesh(0)->GetMaterial()->SetAmbientOcclusionMap(TextureLoader::Load2DTexture("res/cerberus/materials/cerberus_ao.png"));*/


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