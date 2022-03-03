#include "GBuffer.h"

namespace TEngine {
	GBuffer::GBuffer(int width, int height) :
		FrameBuffer(width, height) // just create a empty framebuffer without attachment
	{
		// create multiple render targets
		Init();
	}

	GBuffer::~GBuffer()
	{
		for (int i = 0; i < mRenderTargetsCount; i++)
			delete mRenderTargets[i];
	}

	void GBuffer::Clear()
	{
		glm::vec4 positionClear(100000.0f);
		glm::vec4 normalClear(0.0f);
		glm::vec4 mixtureClear(0.0f);

		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearBufferfv(GL_COLOR, 1, &normalClear[0]);
		glClearBufferfv(GL_COLOR, 2, &positionClear[0]);
		glClearBufferfv(GL_COLOR, 3, &mixtureClear[0]);
	}

	void GBuffer::ResizeFrameBuffer(int width, int height)
	{
		mWidth = width;
		mHeight = height;

		// resize all 4 rendertargets
		for (int i = 0; i < mRenderTargetsCount; i++)
		{
			Texture* current = mRenderTargets[i];
			current->bind(0);
			const TextureSettings& settings = current->GetTextureSettings();
			glTexImage2D(current->GetTextureTarget(), 0, settings.TextureFormat, width, height, 0, settings.dataFormat, settings.dataType, nullptr);
			current->unbind();
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

	void GBuffer::Init()
	{
		// create depth texture settings and add depth attachment to framebuffer
		// We create depth texture attachment here for future use  
		TextureSettings depthStencilTextureSettings;
		depthStencilTextureSettings.TextureFormat = GL_DEPTH24_STENCIL8;
		depthStencilTextureSettings.TextureWrapSMode = GL_CLAMP_TO_BORDER;
		depthStencilTextureSettings.TextureWrapTMode = GL_CLAMP_TO_BORDER;
		depthStencilTextureSettings.TextureMagnificationFilterMode = GL_NEAREST;
		depthStencilTextureSettings.TextureMinificationFilterMode = GL_NEAREST;
		depthStencilTextureSettings.HasBorder = true;	// use default (1,1,1) border color
		depthStencilTextureSettings.HasMips = false;
		AddDepthStencilTexture(depthStencilTextureSettings, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL_ATTACHMENT);

		// create 4 render targets
		Bind();
		mRenderTargetsCount = 4;

		TextureSettings renderTarget1;
		renderTarget1.TextureFormat = GL_RGBA8;
		renderTarget1.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		renderTarget1.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		renderTarget1.TextureMinificationFilterMode = GL_NEAREST;
		renderTarget1.TextureMagnificationFilterMode = GL_NEAREST;
		renderTarget1.HasMips = false;
		Texture* albedoTargets = new Texture(renderTarget1);
		albedoTargets->Generate2DTexture(mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE);
		mRenderTargets.push_back(albedoTargets);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderTargets[0]->GetTextureId(), 0);


		TextureSettings renderTarget2;
		renderTarget2.TextureFormat = GL_RGB32F;
		renderTarget2.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		renderTarget2.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		renderTarget2.TextureMinificationFilterMode = GL_NEAREST;
		renderTarget2.TextureMagnificationFilterMode = GL_NEAREST;
		renderTarget2.HasMips = false;
		Texture* normalTargets = new Texture(renderTarget2);
		normalTargets->Generate2DTexture(mWidth, mHeight, GL_RGB, GL_FLOAT);
		mRenderTargets.push_back(normalTargets);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mRenderTargets[1]->GetTextureId(), 0);

		TextureSettings renderTarget3;
		renderTarget3.TextureFormat = GL_RGB32F;
		renderTarget3.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		renderTarget3.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		renderTarget3.TextureMinificationFilterMode = GL_NEAREST;
		renderTarget3.TextureMagnificationFilterMode = GL_NEAREST;
		renderTarget3.HasMips = false;
		Texture* positionTargets = new Texture(renderTarget3);
		positionTargets->Generate2DTexture(mWidth, mHeight, GL_RGB, GL_FLOAT);
		mRenderTargets.push_back(positionTargets);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mRenderTargets[2]->GetTextureId(), 0);

		TextureSettings renderTarget4;
		renderTarget4.TextureFormat = GL_RGB8;
		renderTarget4.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		renderTarget4.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		renderTarget4.TextureMinificationFilterMode = GL_NEAREST;
		renderTarget4.TextureMagnificationFilterMode = GL_NEAREST;
		renderTarget4.HasMips = false;
		Texture* mixtureTargets = new Texture(renderTarget2);
		mixtureTargets->Generate2DTexture(mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE);
		mRenderTargets.push_back(mixtureTargets);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mRenderTargets[3]->GetTextureId(), 0);

		// tell OpenGL that we will be rendering to all of the attachments
		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);

		// check if the creation failed
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "INITIALIZE GBFFER FAIL!\n";
			return;
		}

		UnBind();
	}
}