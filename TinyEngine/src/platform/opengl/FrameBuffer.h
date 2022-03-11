#pragma once
#include "./glad/glad.h"
#include "../../graphics/texture/Texture.h"
namespace TEngine {

	// TODO: this framebuffer structure may need to change, i.e. the add attachment functions are not convinent now
	class FrameBuffer
	{
	public:
		FrameBuffer(int width, int height, bool isMultiSample = false);
		virtual ~FrameBuffer();

		void CreateFrameBuffer();
		void AddColorTexture(const TextureSettings& textureSettings, GLenum dataFormat, GLenum dataType, GLenum attachType);
		void AddDepthStencilTexture(const TextureSettings& textureSettings, GLenum dataFormat, GLenum dataType, GLenum attachType);
		void AddDepthStencilRBO(GLenum rboFormat, GLenum attachType);

		// When user changes the window size, we need to resize framebuffer too.
		virtual void ResizeFrameBuffer(int width, int height);

		virtual void Clear();
		void Bind();
		void UnBind();
		void SaverRenderFrame(const std::string& savePath);
		// getters
		inline int GetWidth() { return mWidth; }
		inline int GetHeight() { return mHeight; }

		inline unsigned int GetFramebuffer() { return mFBO; }
		inline unsigned int GetDepthStencilRBO() { return mDepthStencilRBO; }
		inline Texture* GetColourTexture() { return &mColorTexture; }
		inline Texture* GetDepthStencilTexture() { return &mDepthStencilTexture; }
		
	protected:
		// framebuffer's property
		unsigned int mFBO;
		int mWidth, mHeight;
		bool mIsMultiSample;

		// default framebuffer's attachment
		Texture mColorTexture;
		Texture mDepthStencilTexture;

		GLenum mRBOFormat;
		unsigned int mDepthStencilRBO;
		bool mIsUseRBO;
	};
}

