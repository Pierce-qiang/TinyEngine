#pragma once

#include <vector>
#include <iostream>
#include "../../graphics/texture/Texture.h"
#include "FrameBuffer.h"
namespace TEngine {
	class GBuffer :public FrameBuffer
	{
	public:
		GBuffer(int width, int height);
		~GBuffer();

		virtual void Clear();
		virtual void ResizeFrameBuffer(int width, int height);


		inline Texture* GetRenderTarget(int index) { return mRenderTargets[index]; }

	private:
		void Init();

	private:
		int mRenderTargetsCount;


		// 0 RGBA8  ->       albedo.r     albedo.g        albedo.b     albedo's alpha
		// 1 RGB32F ->       normal.x     normal.y        normal.z
		// 2 RGB32F ->       pos.x		  pos.y		      pos.z
		// 3 RGB8  ->        roughness    metallic        ambientOcclusion
		std::vector<Texture*> mRenderTargets;
	};
}


