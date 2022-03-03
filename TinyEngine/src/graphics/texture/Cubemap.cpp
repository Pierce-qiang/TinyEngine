#include "Cubemap.h"

namespace TEngine
{
	Cubemap::Cubemap(const CubemapSettings& settings) :
		mCubemapID(0), mWidth(0), mHeight(0),
		mGeneratedFacesNum(0), mSettings(settings)
	{
	}

	Cubemap::~Cubemap()
	{
		glDeleteTextures(1, &mCubemapID);
	}

	void Cubemap::Bind(int unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemapID);
	}

	void Cubemap::UnBind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void Cubemap::GenerateCubemapFace(GLenum face, unsigned int width, unsigned int height, GLenum dataFormat, GLenum pixelDataType, unsigned char* data)
	{
		if (mCubemapID == 0)
		{
			glGenTextures(1, &mCubemapID);
			mWidth = width;
			mHeight = height;

			if (mSettings.TextureFormat == GL_NONE)
				mSettings.TextureFormat = dataFormat;

			if (mSettings.IsSRGB == true)
			{
				switch (mSettings.TextureFormat)
				{
				case GL_RGB:
					mSettings.TextureFormat = GL_SRGB; break;
				case GL_RGBA:
					mSettings.TextureFormat = GL_SRGB_ALPHA; break;
				}
			}
		}

		Bind();

		glTexImage2D(face, 0, mSettings.TextureFormat, width, height, 0, dataFormat, pixelDataType, data);
		++mGeneratedFacesNum;

		if (mGeneratedFacesNum >= 6)
			ApplyCubemapSettings();


		UnBind();
	}

	void Cubemap::ApplyCubemapSettings()
	{
		// Texture wrapping
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, mSettings.TextureWrapSMode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, mSettings.TextureWrapTMode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, mSettings.TextureWrapRMode);

		// Texture filtering
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mSettings.TextureMagnificationFilterMode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mSettings.TextureMinificationFilterMode);

		// Mipmapping
		if (mSettings.HasMips)
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_LOD_BIAS, mSettings.MipBias);
		}
	}
}