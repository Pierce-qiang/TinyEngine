#pragma once

#include "../texture/Texture.h"
#include "../Shader.h"
namespace TEngine {
	class Material
	{
	public:
		Material(Texture* albedoMap = nullptr, Texture* normalMap = nullptr, Texture* metallicMap = nullptr, Texture* roughnessMap = nullptr,
			Texture* ambientOcclusionMap = nullptr, Texture* mixtureMap = nullptr);
		~Material();

		void SeperateMixture();
		void BindMaterial(Shader* shader) const;

		inline void SetAlbedoMap(Texture* texture) { mAlbedoMap = texture; }
		inline void SetNormalMap(Texture* texture) { mNormalMap = texture; }
		inline void SetMetallicMap(Texture* texture) { mMetallicMap = texture; }
		inline void SetRoughnessMap(Texture* texture) { mRoughnessMap = texture; }
		inline void SetAmbientOcclusionMap(Texture* texture) { mAOMap = texture; }
		inline void SetMixtureMap(Texture* texture) { mMixtureMap = texture; }

	private:
		// TODO: remember to release this texture inside or outside
		Texture* mAlbedoMap;
		Texture* mNormalMap;
		Texture* mMetallicMap;
		Texture* mRoughnessMap;
		Texture* mAOMap;
		Texture* mMixtureMap; // this is for gltf format model

	};
}



