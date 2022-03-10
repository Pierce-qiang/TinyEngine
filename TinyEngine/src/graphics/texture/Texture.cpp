#include "Texture.h"
#include "../../macros.h"
namespace TEngine {
	Texture::Texture(const TextureSettings& settings /*= TextureSettings()*/) :
		mTextureId(0), mTextureTarget(0), mWidth(0), mHeight(0), mTextureSettings(settings)
	{
	}

	Texture::~Texture()
	{
		if (mTextureId != 0)
			glDeleteTextures(1, &mTextureId);
	}

	void Texture::Generate2DTexture(unsigned int width, unsigned int height, GLenum dataFormat, GLenum pixelDataType, const void* data)
	{
		mTextureTarget = GL_TEXTURE_2D;
		mWidth = width;
		mHeight = height;

		// If GL_NONE is specified, set the texture format to the data format
		if (mTextureSettings.TextureFormat == GL_NONE)
			mTextureSettings.TextureFormat = dataFormat;

		// Check if the texture is SRGB, if so change the texture format and let gpu do the gamma correction
		if (mTextureSettings.IsSRGB)
		{
			switch (dataFormat)
			{
			case GL_RGB: mTextureSettings.TextureFormat = GL_SRGB; break;
			case GL_RGBA: mTextureSettings.TextureFormat = GL_SRGB_ALPHA; break;
			}
		}

		// generate texture and setup basics
		glGenTextures(1, &mTextureId);
		bind();
		glTexImage2D(GL_TEXTURE_2D, 0, mTextureSettings.TextureFormat, width, height, 0, dataFormat, pixelDataType, data);

		// record data format and data type
		mTextureSettings.dataFormat = dataFormat;
		mTextureSettings.dataType = pixelDataType;

		// appy settings
		ApplyTextureSettings();
		unbind();
	}

	void Texture::Generate2DMultisampleTexture(unsigned int width, unsigned int height)
	{
		// multisampled textures do not support mips or filtering/wrapping options
		mTextureTarget = GL_TEXTURE_2D_MULTISAMPLE;
		mWidth = width;
		mHeight = height;

		glGenTextures(1, &mTextureId);
		bind();
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLE_AMOUNT, mTextureSettings.TextureFormat, mWidth, mHeight, GL_TRUE);
		unbind();
	}

	void Texture::GenerateMips()
	{
		mTextureSettings.HasMips = true;
		if (IsGenerated())
		{
			bind();
			glGenerateMipmap(mTextureTarget);
			unbind();
		}
	}

	void Texture::bind(int unit /*= 0*/)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(mTextureTarget, mTextureId);
	}


	void Texture::unbind()
	{
		glBindTexture(mTextureTarget, 0);
	}

	void Texture::ApplyTextureSettings()
	{
		// Texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mTextureSettings.TextureWrapSMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mTextureSettings.TextureWrapTMode);
		if (mTextureSettings.HasBorder)
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &mTextureSettings.BorderColour[0]);


		// Texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mTextureSettings.TextureMinificationFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mTextureSettings.TextureMagnificationFilterMode);

		// Mipmapping
		if (mTextureSettings.HasMips)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, mTextureSettings.MipBias);
		}
	}
	void Texture::displayTexture(std::string name, bool invert /*= true*/, ImGuiWindowFlags flag /* = ImGuiWindowFlags()*/) {

		ImGui::Begin(name.c_str(),0,flag);
		ImGui::Text("size = %d x %d", mWidth, mHeight);
		if(invert)
			ImGui::Image((void*)(intptr_t)mTextureId, ImVec2(mWidth, mHeight), ImVec2(0, 1), ImVec2(1, 0));
		else ImGui::Image((void*)(intptr_t)mTextureId, ImVec2(mWidth, mHeight));
		ImGui::End();
	}
}