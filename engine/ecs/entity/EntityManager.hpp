#pragma once

#include <vector>
#include <unordered_map>

#include "Entity.hpp"

class EntityManager
{
public:
    Entity createEntity();
    void destroyEntity(Entity e);

	Entity getEntity(const std::string& uuid) const;
	std::string getUUID(Entity e) const;
	void generateUUID(Entity e);

    const std::vector<Entity>& aliveEntities() const;

private:
    Entity nextEntity = 1;

    std::vector<Entity> m_aliveEntities;
    std::unordered_map<Entity, std::string> m_entityToUUID;
    std::unordered_map<std::string, Entity> m_uuidToEntity;
};