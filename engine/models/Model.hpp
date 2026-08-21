#pragma once

#include <assets/IAsset.hpp>
#include <animation/Skeleton.hpp>
#include <animation/Animation.hpp>
#include <render/mesh/Mesh.hpp>

#include <memory>
#include <vector>
#include <string>
#include <editor/DrawTypes.h>
#include <render/mesh/AABB.hpp>

class Material;



class Model : public IAsset
{
public:
	Model(std::vector<std::shared_ptr<Mesh>> meshes) : meshes(meshes) { initializeModelAABB(*this); }
    Model() = default;
    ~Model() = default;

    std::string getUUID() { return m_uuid; }
    void setUUID(std::string uuid) { m_uuid = uuid; }

    void draw();

    void buildPhysicsCache(const std::vector<IndexedMesh>& rawMeshes);

    uint32_t getVertexCount() const;
    uint32_t getVertexStride() const;
    const void* getVertexData() const;

    uint32_t getIndexCount() const;
    uint32_t getIndexStride() const;
    const void* getIndexData() const;

    std::vector<std::shared_ptr<Mesh>> meshes;
    AABB bounds;
    AABB localAABB;
    
    static void initializeModelAABB(Model& model);

    std::vector<Skeleton> skeletons;
    std::vector<AnimationClip> animations;

protected:
    std::string m_uuid;
    std::string m_name;

private:
    std::vector<Vertex> cachedVertices;
    std::vector<uint32_t> cachedIndices;
};

