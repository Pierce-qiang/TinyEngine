#include "TextureLoader.h"
#include <iostream>

#include "./stb_image.h"

namespace TEngine
{
	// We must define the static variables outside the class first to get memory
	std::unordered_map<std::string, Texture*> TextureLoader::mTextureCache;
	Texture* TextureLoader::sDefaultAlbedo = nullptr;
	Texture* TextureLoader::sDefaultNormal = nullptr;
	Texture* TextureLoader::sWhiteTexture = nullptr;
	Texture* TextureLoader::sBlackTexture = nullptr;

	void TextureLoader::InitDefaultTextures()
	{
		// Setup texture and minimal filtering because they are 1x1 textures so they require none
		TextureSettings srgbTextureSettings, formalTextureSettings;
		srgbTextureSettings.IsSRGB = true;
		srgbTextureSettings.TextureMinificationFilterMode = GL_NEAREST;
		srgbTextureSettings.TextureMagnificationFilterMode = GL_NEAREST;
		srgbTextureSettings.HasMips = false;
		formalTextureSettings.IsSRGB = false;
		formalTextureSettings.TextureMinificationFilterMode = GL_NEAREST;
		formalTextureSettings.TextureMagnificationFilterMode = GL_NEAREST;
		formalTextureSettings.HasMips = false;

		sDefaultAlbedo = Load2DTexture(std::string("res/texture/defaultAlbedo.png"), &srgbTextureSettings);
		sDefaultNormal = Load2DTexture(std::string("res/texture/defaultNormal.png"), &formalTextureSettings);
		sWhiteTexture = Load2DTexture(std::string("res/texture/white.png"), &formalTextureSettings);
		sBlackTexture = Load2DTexture(std::string("res/texture/black.png"), &formalTextureSettings);
	}

	Texture* TextureLoader::Load2DTexture(const std::string& path, TextureSettings* settings)
	{
		// check the cache
		auto iter = mTextureCache.find(path);
		if (iter != mTextureCache.end())
			return iter->second;


		// load texture
		int width, height, numComponents;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &numComponents, 0);
		if (!data)
		{
			std::cout << "TEXTURE LOAD FAIL - path:" << path << "\n";
			stbi_image_free(data);
			return nullptr;
		}

		GLenum dataFormat;
		switch (numComponents)
		{
		case 1: dataFormat = GL_RED;  break;
		case 3: dataFormat = GL_RGB;  break;
		case 4: dataFormat = GL_RGBA; break;
		}

		// use the specified of default settings to initialize texture
		Texture* texture = nullptr;
		if (settings != nullptr)
			texture = new Texture(*settings);
		else
			texture = new Texture();

		texture->GetTextureSettings().ChannelNum = numComponents;
		texture->Generate2DTexture(width, height, dataFormat, GL_UNSIGNED_BYTE, data);

		mTextureCache.insert(std::pair<std::string, Texture*>(path, texture));
		stbi_image_free(data);

		std::cout << path << " load successfully!\n";
		return mTextureCache[path];
	}

	Cubemap* TextureLoader::LoadCubemapTexture(const std::vector<std::string>& paths, CubemapSettings* settings)
	{
		// use the specified of default settings to initialize texture
		Cubemap* cubemap = nullptr;
		if (settings != nullptr)
			cubemap = new Cubemap(*settings);
		else
			cubemap = new Cubemap();

		int width, height, numComponents;
		for (size_t i = 0; i < paths.size(); i++)
		{
			unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &numComponents, 0);

			if (data)
			{
				GLenum dataFormat;
				switch (numComponents)
				{
				case 1: dataFormat = GL_RED;  break;
				case 3: dataFormat = GL_RGB;  break;
				case 4: dataFormat = GL_RGBA; break;
				}

				std::cout << paths[i] << " load successfully!\n";
				cubemap->GenerateCubemapFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "TEXTURE LOAD FAIL - path:" << paths[i] << "\n";
				stbi_image_free(data);
				return nullptr;
			}
		}

		return cubemap;
	}

	void TextureLoader::DestroyCachedTexture()
	{
		for (auto& item : mTextureCache)
			delete item.second;
	}
}