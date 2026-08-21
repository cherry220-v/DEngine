#include "EntityManager.hpp"
#include <core/GUUID.h>
#include <stdexcept>

Entity EntityManager::createEntity()
{
	Entity e = nextEntity++;
	m_aliveEntities.push_back(e);
	generateUUID(e);
	return e;
}

void EntityManager::destroyEntity(Entity e)
{
	auto it = std::find(m_aliveEntities.begin(), m_aliveEntities.end(), e);
	if (it != m_aliveEntities.end())
	{
		m_aliveEntities.erase(it);
		m_entityToUUID.erase(e);
		m_uuidToEntity.erase(getUUID(e));
	}
}

Entity EntityManager::getEntity(const std::string& uuid) const
{
	auto it = m_uuidToEntity.find(uuid);
	if (it != m_uuidToEntity.end())
	{
		return it->second;
	}
	throw std::runtime_error("Entity not found");
}

std::string EntityManager::getUUID(Entity e) const
{
	auto it = m_entityToUUID.find(e);
	if (it != m_entityToUUID.end())
	{
		return it->second;
	}
	throw std::runtime_error("UUID not found");
}

void EntityManager::generateUUID(Entity e)
{
	std::string uuid = GUUID::generate();
	m_entityToUUID[e] = uuid;
	m_uuidToEntity[uuid] = e;
}

const std::vector<Entity>& EntityManager::aliveEntities() const
{
	return m_aliveEntities;
}