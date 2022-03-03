#include "FrameBuffer.h"
#include <iostream>
namespace TEngine {
	FrameBuffer::FrameBuffer(int width, int height, bool isMultiSample) :
		mWidth(width), mHeight(height), mIsMultiSample(isMultiSample)
	{
		mIsUseRBO = false;
		glGenFramebuffers(1, &mFBO);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &mFBO);
		if (mIsUseRBO)
			glDeleteRenderbuffers(1, &mDepthStencilRBO);
	}

	void FrameBuffer::CreateFrameBuffer()
	{
		Bind();
		if (!mColorTexture.IsGenerated())
		{
			// indicate that there won't be a colour buffer for this FBO
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		// check if the creation failed
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "CREATE FRAMEBUFFER FAILED!\n";
			return;
		}
		UnBind();

	}

	void FrameBuffer::AddColorTexture(const TextureSettings& textureSettings, GLenum dataFormat, GLenum dataType, GLenum attachType)
	{
		// bind framebuffer first
		Bind();

		//// create the color texture with settings and attach it to framebuffer
		//TextureSettings colorTextureSettings;
		//colorTextureSettings.TextureFormat = textureFormat;
		//colorTextureSettings.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		//colorTextureSettings.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		//colorTextureSettings.TextureMinificationFilterMode = GL_LINEAR;
		//colorTextureSettings.TextureMagnificationFilterMode = GL_LINEAR;
		//colorTextureSettings.HasMips = false;

		mColorTexture.SetTextureSettings(textureSettings);
		mColorTexture.Generate2DTexture(mWidth, mHeight, dataFormat, dataType);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachType, GL_TEXTURE_2D, mColorTexture.GetTextureId(), 0);

		UnBind();
	}

	void FrameBuffer::AddDepthStencilTexture(const TextureSettings& textureSettings, GLenum dataFormat, GLenum dataType, GLenum attachType)
	{
		// bind framebuffer first
		Bind();

		//// create the depth/stencil texture with settings and attach it to framebuffer
		//TextureSettings depthStencilSettings;
		//depthStencilSettings.TextureFormat = textureFormat;
		//depthStencilSettings.TextureWrapSMode = GL_CLAMP_TO_BORDER;
		//depthStencilSettings.TextureWrapTMode = GL_CLAMP_TO_BORDER;
		//depthStencilSettings.TextureMinificationFilterMode = GL_NEAREST;
		//depthStencilSettings.TextureMagnificationFilterMode = GL_NEAREST;
		//depthStencilSettings.HasBorder = true;
		//depthStencilSettings.HasMips = false;

		mDepthStencilTexture.SetTextureSettings(textureSettings);
		mDepthStencilTexture.Generate2DTexture(mWidth, mHeight, dataFormat, dataType);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachType, GL_TEXTURE_2D, mDepthStencilTexture.GetTextureId(), 0);

		UnBind();
	}

	// e.g.   GL_DEPTH24_STENCIL8  ,  GL_DEPTH_STENCIL_ATTACHMENT
	void FrameBuffer::AddDepthStencilRBO(GLenum rboFormat, GLenum attachType)
	{
		mRBOFormat = rboFormat;
		mIsUseRBO = true;
		Bind();

		glGenRenderbuffers(1, &mDepthStencilRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, rboFormat, mWidth, mHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachType, GL_RENDERBUFFER, mDepthStencilRBO);

		UnBind();
	}

	//glTexImage2D(GL_TEXTURE_2D, 0, mTextureSettings.TextureFormat, width, height, 0, dataFormat, pixelDataType, data);
	void FrameBuffer::ResizeFrameBuffer(int width, int height)
	{
		mWidth = width;
		mHeight = height;

		if (mColorTexture.IsGenerated())
		{
			mColorTexture.bind(0);
			const TextureSettings& settings = mColorTexture.GetTextureSettings();
			glTexImage2D(mColorTexture.GetTextureTarget(), 0, settings.TextureFormat, width, height, 0, settings.dataFormat, settings.dataType, nullptr);
			mColorTexture.unbind();
		}

		if (mDepthStencilTexture.IsGenerated())
		{
			mDepthStencilTexture.bind(0);
			const TextureSettings& settings = mDepthStencilTexture.GetTextureSettings();
			glTexImage2D(mDepthStencilTexture.GetTextureTarget(), 0, settings.TextureFormat, width, height, 0, settings.dataFormat, settings.dataType, nullptr);
			mDepthStencilTexture.unbind();
		}

		if (mIsUseRBO)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, mRBOFormat, mWidth, mHeight);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}

	void FrameBuffer::Clear()
	{
		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	}

	void FrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}