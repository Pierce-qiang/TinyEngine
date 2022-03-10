#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "RenderPassManager.h"
#include "stb_image.h"
#include "../../platform/window/WindowManager.h"
#include "stb_image_write.h"
namespace TEngine {
	RenderPassManager::RenderPassManager(Scene* scene) :
		mScene(scene), mFowardLightingPass(scene), mShadowmapPass(scene),
		mDeferredGeometryPass(scene), mDeferredLightingPass(scene), mSSRPass(scene),
		mFXAAPass(scene), mPS(scene)
	{
		// always enable this to let sample cube map seamlessly
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	RenderPassManager::~RenderPassManager()
	{

	}

	void RenderPassManager::Render()
	{
		// it's a simple test now
		//ShadowmapPassOutput smOutput = mShadowmapPass.Render();
		//mFowardLightingPass.Render(smOutput, true);

		ShadowmapPassOutput smOutput = mShadowmapPass.Render();
		GeometryPassOutput gOutput = mDeferredGeometryPass.Render();
		LightingPassOutput lightOutput = mDeferredLightingPass.Render(gOutput, smOutput);
		SSRPassOutput ssrOutput = mSSRPass.Render(gOutput, lightOutput);
		PostProcessOutput fxaaOutput = mFXAAPass.Render(lightOutput);

		//mPS.Render(gOutput, lightOutput.deferredLightingFBO, ssrOutput);
		mPS.Render(gOutput, fxaaOutput.fbo, ssrOutput, smOutput);
	}

	void RenderPassManager::SaverRenderFrame(const std::string& savePath)
	{
		int width = WindowManager::Instance()->GetWidth();
		int height = WindowManager::Instance()->GetHeight();
		WindowManager::Bind();

		unsigned char* data = new unsigned char[width * height * 3];
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

		//for (int i = 0; i < width * height; i++)
		//{
		//	std::cout << (int)data[i * 3] << " " << (int)data[i * 3 + 1] << " " << (int)data[i * 3 + 2] << "\n";
		//}

		// flip texture
		unsigned char* flippedData = new unsigned char[width * height * 3];
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int index1 = (i * width + j) * 3;
				int index2 = ((height - 1 - i) * width + j) * 3;
				flippedData[index1] = data[index2];
				flippedData[index1 + 1] = data[index2 + 1];
				flippedData[index1 + 2] = data[index2 + 2];
			}
		}

		stbi_write_png(savePath.c_str(), width, height,
			3, flippedData, width * 3);

		free(data);
	}
}