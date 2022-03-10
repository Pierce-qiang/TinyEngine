#pragma once
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "imgui/imgui.h"
#include <string>
namespace TEngine {
	struct TextureSettings
	{
		// Texture format
		GLenum TextureFormat = GL_NONE; // If set to GL_NONE, the data format will be used

		/* isSRGB will let the loader know that the texture needs to be "linearlized" before it is sampled in the shaders (i.e the texture is in a non liner space)
		.Note: If you generate your own data and it is already in linear space (like light probes), be careful */
		bool IsSRGB = false;

		// Texture wrapping options
		GLenum TextureWrapSMode = GL_REPEAT;
		GLenum TextureWrapTMode = GL_REPEAT;
		bool HasBorder = false;
		glm::vec4 BorderColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //default border used for shadow map

		// Texture filtering options
		GLenum TextureMinificationFilterMode = GL_LINEAR_MIPMAP_LINEAR; // Filtering mode when the texture moves further away and multiple texels map to one pixel (trilinear for best quality)
		GLenum TextureMagnificationFilterMode = GL_LINEAR; // Filtering mode when the texture gets closer and multiple pixels map to a single texel (Never needs to be more than bilinear because that is as accurate as it gets in this situation)

		// Mip options
		bool HasMips = true;
		int MipBias = 0; // positive means blurrier texture selected, negative means sharper texture which can show texture aliasing

		// Record data type for framebuffer resizing
		GLenum dataFormat = GL_NONE;
		GLenum dataType = GL_NONE;

		int ChannelNum = 0;
	};
	class Texture
	{
	public:
		Texture(const TextureSettings& settings = TextureSettings());
		~Texture();

		// generation functions
		void Generate2DTexture(unsigned int width, unsigned int height, GLenum dataFormat, GLenum pixelDataType = GL_UNSIGNED_BYTE, const void* data = nullptr);
		void Generate2DMultisampleTexture(unsigned int width, unsigned int height);
		void GenerateMips(); // attempt to generate mipmaps, only works if the texture has already been generated

		void bind(int unit = 0);
		void unbind();
		void displayTexture(std::string name, bool invert = true, ImGuiWindowFlags flag = ImGuiWindowFlags());

		// pre-generation controls only
		inline void SetTextureSettings(TextureSettings settings) { mTextureSettings = settings; }
		inline void SetTextureFormat(GLenum format) { mTextureSettings.TextureFormat = format; }

		// Getters
		inline unsigned int GetTextureId() const { return mTextureId; }
		inline unsigned int GetTextureTarget() const { return mTextureTarget; }
		inline bool IsGenerated() const { return mTextureId != 0; }
		inline unsigned int GetWidth() const { return mWidth; }
		inline unsigned int GetHeight() const { return mHeight; }
		inline TextureSettings& GetTextureSettings() { return mTextureSettings; }

	private:
		void ApplyTextureSettings();
	private:
		GLenum mTextureTarget;
		unsigned int mTextureId;
		unsigned int mWidth, mHeight;
		//unsigned int channel;  which is actually data type in struct Texture setting

		TextureSettings mTextureSettings;
	};
}


