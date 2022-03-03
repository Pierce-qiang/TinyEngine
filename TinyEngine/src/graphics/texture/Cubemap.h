#pragma once
#include "glad/glad.h"

namespace TEngine {
	struct CubemapSettings
	{
		// Texture format
		GLenum TextureFormat = GL_NONE; // If set to GL_NONE, the data format will be used

		/* isSRGB will let the loader know that the texture needs to be "linearlized" before it is sampled in the shaders (ie the texture is in a non liner space)
		 * Anything that will be used for colour in a renderer should be linearlized. However textures that contain data (Heightfields, normal maps, metallic maps etc.) should not be,
		 * thus they are not in SRGB space. Note: If you generate your own data and it is already in linear space (like light probes), be careful */
		bool IsSRGB = false;

		// Texture wrapping options
		GLenum TextureWrapSMode = GL_CLAMP_TO_EDGE;
		GLenum TextureWrapTMode = GL_CLAMP_TO_EDGE;
		GLenum TextureWrapRMode = GL_CLAMP_TO_EDGE;

		// Texture filtering options
		GLenum TextureMinificationFilterMode = GL_LINEAR; // Filtering mode when the texture moves further away and multiple texels map to one pixel (trilinear for best quality)
		GLenum TextureMagnificationFilterMode = GL_LINEAR; // Filtering mode when the texture gets closer and multiple pixels map to a single texel (Never needs to be more than bilinear because that is as accurate as it gets in this sitation)

		// Mip Settings
		bool HasMips = false;
		int MipBias = 0; // positive means blurrier texture selected, negative means sharper texture which can show texture aliasing
	};
	class Cubemap
	{
	public:
		Cubemap(const CubemapSettings& settings = CubemapSettings());
		~Cubemap();

		void Bind(int unit = 0);
		void UnBind();

		void GenerateCubemapFace(GLenum face, unsigned int width, unsigned int height, GLenum dataFormat, GLenum pixelDataType = GL_UNSIGNED_BYTE, unsigned char* data = nullptr);



		inline int GetWidth() { return mWidth; }
		inline int GetHeight() { return mHeight; }
		inline CubemapSettings* GetCubemapSettings() { return &mSettings; }
		unsigned int GetCubemapID() { return mCubemapID; }
		inline void SetCubemapSettings(const CubemapSettings& settings) { mSettings = settings; }

	private:
		void ApplyCubemapSettings();
	private:
		unsigned int mCubemapID;
		int mWidth, mHeight;
		int mGeneratedFacesNum;
		CubemapSettings mSettings;

	};
}


