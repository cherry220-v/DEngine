#pragma once

#include "AssetRegistry.hpp"
#include "AssetTypeInfo.hpp"

#include <render/shader/loaders/ShaderLoader.hpp>
#include <models/loaders/GLBLoader.hpp>
#include <models/loaders/OBJLoader.hpp>
#include <render/texture/loaders/MaterialLoader.hpp>
#include <render/texture/loaders/TextureLoader.hpp>

#include <models/Model.hpp>
#include <render/shader/Shader.hpp>
#include <render/texture/Texture.hpp>
#include <render/texture/Material.hpp>

struct AssetLoaderRegistration
{
	static void registerAll()
	{
		auto& assetRegistry = AssetRegistry::get();
		assetRegistry.registerLoader(
			std::vector<std::string>{ ".obj" },
			makeAssetTypeInfo<Model>(AssetType::Model, std::make_shared<OBJLoader>())
		);
		assetRegistry.registerLoader(
			std::vector<std::string>{ ".glb" },
			makeAssetTypeInfo<Model>(AssetType::Model, std::make_shared<GLBLoader>())
		);
		assetRegistry.registerLoader(
			std::vector<std::string>{ ".shader" },
			makeAssetTypeInfo<Shader>(AssetType::Shader, std::make_shared<ShaderLoader>())
		);
		assetRegistry.registerLoader(
			std::vector<std::string>{ ".material" },
			makeAssetTypeInfo<Material>(AssetType::Material, std::make_shared<MaterialLoader>())
		);
		assetRegistry.registerLoader(
			std::vector<std::string>{ ".png", ".jpg", ".jpeg", ".bmp", ".tga" },
			makeAssetTypeInfo<Texture>(AssetType::Texture, std::make_shared<TextureLoader>())
		);
	}
};