#pragma once
#include "Texture.h"
#include <string>
#include <vector>
#include "Cubemap.h"
#include <unordered_map>
namespace TEngine {
	class TextureLoader
	{
	public:
		static void InitDefaultTextures();

		// TODO: HDR loading
		static Texture* Load2DTexture(const std::string& path, TextureSettings* settings = nullptr);
		static Cubemap* LoadCubemapTexture(const std::vector<std::string>& paths, CubemapSettings* settings = nullptr);

		static void DestroyCachedTexture();

		// getters
		static inline Texture* GetWhiteTexture() { return sWhiteTexture; }
		static inline Texture* GetBlackTexture() { return sBlackTexture; }
		static inline Texture* GetDefaultAlbedo() { return sDefaultAlbedo; }
		static inline Texture* GetDefaultNormal() { return sDefaultNormal; }
		static inline Texture* GetDefaultMetallic() { return sBlackTexture; }
		static inline Texture* GetDefaultRoughness() { return sBlackTexture; }
		static inline Texture* GetDefaultAO() { return sWhiteTexture; }
		static inline Texture* GetFullMetallic() { return sWhiteTexture; }
		static inline Texture* GetNoMetallic() { return sBlackTexture; }
		static inline Texture* GetFullRoughness() { return sWhiteTexture; }
		static inline Texture* GetNoRoughness() { return sBlackTexture; }

	private:
		static std::unordered_map<std::string, Texture*> mTextureCache;

		// Default Textures used for test
		static Texture* sDefaultAlbedo;
		static Texture* sDefaultNormal;
		static Texture* sWhiteTexture;
		static Texture* sBlackTexture;
	};
}


