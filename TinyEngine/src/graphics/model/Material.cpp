#include "./Material.h"

#include <iostream>

#include "../texture/TextureLoader.h"
//#include "stb_image_write.h"
#include "stb_image.h"
namespace TEngine {
	Material::Material(Texture* albedoMap, Texture* normalMap, Texture* metallicMap, Texture* roughnessMap, Texture* ambientOcclusionMap, Texture* mixtureMap) :
		mAlbedoMap(albedoMap), mNormalMap(normalMap),
		mMetallicMap(metallicMap), mRoughnessMap(roughnessMap),
		mAOMap(ambientOcclusionMap), mMixtureMap(mixtureMap)
	{
	}

	Material::~Material()
	{
	}

	void Material::SeperateMixture()
	{
		if (mMixtureMap != nullptr)
		{
			int width = mMixtureMap->GetWidth();
			int height = mMixtureMap->GetHeight();
			TextureSettings settings = mMixtureMap->GetTextureSettings();
			int channel = settings.ChannelNum;
			unsigned char* mixtureData = new unsigned char[width * height * channel];
			unsigned char* metalData = new unsigned char[width * height];
			unsigned char* roughData = new unsigned char[width * height];
			unsigned char* aoData = new unsigned char[width * height];
			//std::cout << "Mexture Channel:" << channel << "\n";
			mMixtureMap->bind(0);
			glGetTexImage(GL_TEXTURE_2D, 0, settings.dataFormat, GL_UNSIGNED_BYTE, mixtureData);
			mMixtureMap->unbind();

			//stbi_write_png("test.png", width, height,
			//	3, mixtureData, width * 3);
			if (channel == 1)
			{
				std::cerr << "WRONG MIXTURE FORMATE: Only one channel mixture\n";
				return;
			}


			// for gltf r - ao, g - roughness, b - metallic
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					int index1 = (i * width + j) * channel;
					int index2 = (i * width + j);
					metalData[index2] = mixtureData[index1 + 2];
					roughData[index2] = mixtureData[index1 + 1];
					if (channel > 2)
						aoData[index2] = mixtureData[index1];
				}
			}
			mMetallicMap = new Texture();
			mMetallicMap->Generate2DTexture(width, height, GL_RED, GL_UNSIGNED_BYTE, metalData);
			delete[] metalData;
			mRoughnessMap = new Texture();
			mRoughnessMap->Generate2DTexture(width, height, GL_RED, GL_UNSIGNED_BYTE, roughData);
			delete[] roughData;
			if (channel > 2)
			{
				mAOMap = new Texture();
				mAOMap->Generate2DTexture(width, height, GL_RED, GL_UNSIGNED_BYTE, aoData);
				delete[] aoData;
			}
		}
		else
		{
			std::cerr << "No mixture map has been set, sperate muxture failed!\n ";
		}
	}

	void Material::BindMaterial(Shader* shader) const
	{
		// shader need to be bound outside before binding the material textures

		// Texture unit 0 is reserved for the shadowmap
		// Texture unit 1 is reserved for the irradianceMap used for indirect diffuse IBL
		// Texture unit 2 is reserved for the prefilterMap
		// Texture unit 3 is reserved for the brdfLUT
		int currentTextureUnit = 4;

		shader->SetUniform("material.texture_albedo", currentTextureUnit);
		if (mAlbedoMap)
			mAlbedoMap->bind(currentTextureUnit++);
		else
			TextureLoader::GetDefaultAlbedo()->bind(currentTextureUnit++);


		shader->SetUniform("material.texture_normal", currentTextureUnit);
		if (mNormalMap)
			mNormalMap->bind(currentTextureUnit++);
		else
			TextureLoader::GetDefaultNormal()->bind(currentTextureUnit++);

		shader->SetUniform("material.texture_metallic", currentTextureUnit);
		if (mMetallicMap)
			mMetallicMap->bind(currentTextureUnit++);
		else
			TextureLoader::GetDefaultMetallic()->bind(currentTextureUnit++);

		shader->SetUniform("material.texture_roughness", currentTextureUnit);
		if (mRoughnessMap)
			mRoughnessMap->bind(currentTextureUnit++);
		else
			TextureLoader::GetDefaultRoughness()->bind(currentTextureUnit++);

		shader->SetUniform("material.texture_ao", currentTextureUnit);
		if (mAOMap)
			mAOMap->bind(currentTextureUnit++);
		else
			TextureLoader::GetDefaultAO()->bind(currentTextureUnit++);


		//shader->SetUniform("material.texture_displacement", currentTextureUnit);
		//if (mDisplacementMap)
		//{
		//	shader->SetUniform("hasDisplacement", true);
		//	shader->SetUniform("minMaxDisplacementSteps", glm::vec2(mParallaxMinSteps, mParallelMaxSteps));
		//	shader->SetUniform("parallaxStrength", mParallaxStrength);
		//	mDisplacementMap->bind(currentTextureUnit++);
		//}
		//else 
		//	shader->SetUniform("hasDisplacement", false);
	}
}