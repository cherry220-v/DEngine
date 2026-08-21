#include "AssetMeta.hpp"
#include "AssetTypeInfo.hpp"

nlohmann::json serializeProperty(void* data, AssetType type )
{
	return nlohmann::json();
}

void deserializeProperty(void* data, AssetType type, const nlohmann::json& json )
{
}
