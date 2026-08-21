#include "GLBLoader.hpp"

#include <stdexcept>

#include "../Model.hpp"
#include <render/mesh/Mesh.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math/Vertex.h>
#include <math/VertexKey.h>

static const unsigned char* getBufferData(
    const tinygltf::Model& model,
    const tinygltf::Accessor& accessor)
{
    const auto& bufferView =
        model.bufferViews[accessor.bufferView];

    const auto& buffer =
        model.buffers[bufferView.buffer];

    return buffer.data.data()
        + bufferView.byteOffset
        + accessor.byteOffset;
}

std::shared_ptr<IAsset> GLBLoader::load(const std::string& path)
{
    std::vector<std::shared_ptr<Mesh>> meshes;

    tinygltf::TinyGLTF loader;
    tinygltf::Model gltf;

    std::string error;
    std::string warning;

    bool success =
        loader.LoadBinaryFromFile(
            &gltf,
            &error,
            &warning,
            path);

    if (!warning.empty())
    {
        printf("GLTF Warning: %s\n", warning.c_str());
    }

    if (!success)
    {
        throw std::runtime_error(
            "Failed to load glb: " + error);
    }

    for (const auto& mesh : gltf.meshes)
    {
        AABB bounds;
        for (const auto& primitive : mesh.primitives)
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            const uint16_t* joints = nullptr;
            const float* weights = nullptr;

            auto posIt =
                primitive.attributes.find("POSITION");

            auto jointsIt = primitive.attributes.find("JOINTS_0");
            auto weightsIt = primitive.attributes.find("WEIGHTS_0");

            if (posIt == primitive.attributes.end())
                continue;

            const auto& posAccessor =
                gltf.accessors[posIt->second];

            if (posAccessor.minValues.size() == 3 &&
                posAccessor.maxValues.size() == 3)
            {
                glm::vec3 min(
                    static_cast<float>(posAccessor.minValues[0]),
                    static_cast<float>(posAccessor.minValues[1]),
                    static_cast<float>(posAccessor.minValues[2]));

                glm::vec3 max(
                    static_cast<float>(posAccessor.maxValues[0]),
                    static_cast<float>(posAccessor.maxValues[1]),
                    static_cast<float>(posAccessor.maxValues[2]));

                bounds = { min, max };
            }

            const float* positions =
                reinterpret_cast<const float*>(
                    getBufferData(gltf, posAccessor));

            const float* normals = nullptr;
            const float* uvs = nullptr;

            auto normalIt =
                primitive.attributes.find("NORMAL");

            if (jointsIt != primitive.attributes.end())
            {
                joints = reinterpret_cast<const uint16_t*>(
                    getBufferData(gltf, gltf.accessors[jointsIt->second]));
            }

            if (weightsIt != primitive.attributes.end())
            {
                weights = reinterpret_cast<const float*>(
                    getBufferData(gltf, gltf.accessors[weightsIt->second]));
            }

            if (normalIt != primitive.attributes.end())
            {
                normals =
                    reinterpret_cast<const float*>(
                        getBufferData(
                            gltf,
                            gltf.accessors[normalIt->second]
                        ));
            }

            auto uvIt =
                primitive.attributes.find("TEXCOORD_0");

            if (uvIt != primitive.attributes.end())
            {
                uvs =
                    reinterpret_cast<const float*>(
                        getBufferData(
                            gltf,
                            gltf.accessors[uvIt->second]
                        ));
            }

            vertices.resize(posAccessor.count);

            for (size_t i = 0; i < posAccessor.count; i++)
            {
                Vertex v;

                v.position =
                {
                    positions[i * 3 + 0],
                    positions[i * 3 + 1],
                    positions[i * 3 + 2]
                };

                if (normals)
                {
                    v.normal =
                    {
                        normals[i * 3 + 0],
                        normals[i * 3 + 1],
                        normals[i * 3 + 2]
                    };
                }

                if (uvs)
                {
                    v.uv =
                    {
                        uvs[i * 2 + 0],
                        uvs[i * 2 + 1]
                    };
                }

                if (joints && weights)
                {
                    v.boneIds =
                    {
                        (int)joints[i * 4 + 0],
                        (int)joints[i * 4 + 1],
                        (int)joints[i * 4 + 2],
                        (int)joints[i * 4 + 3]
                    };

                    v.weights =
                    {
                        weights[i * 4 + 0],
                        weights[i * 4 + 1],
                        weights[i * 4 + 2],
                        weights[i * 4 + 3]
                    };
                }

                vertices[i] = v;
            }

            if (primitive.indices >= 0)
            {
                const auto& indexAccessor =
                    gltf.accessors[
                        primitive.indices];

                const unsigned char* data =
                    getBufferData(
                        gltf,
                        indexAccessor);

                indices.resize(indexAccessor.count);

                switch (indexAccessor.componentType)
                {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                {
                    auto ptr =
                        reinterpret_cast<
                        const uint16_t*>(data);

                    for (size_t i = 0;
                        i < indexAccessor.count;
                        i++)
                    {
                        indices[i] = ptr[i];
                    }

                    break;
                }

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                {
                    auto ptr =
                        reinterpret_cast<
                        const uint32_t*>(data);

                    for (size_t i = 0;
                        i < indexAccessor.count;
                        i++)
                    {
                        indices[i] = ptr[i];
                    }

                    break;
                }

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                {
                    auto ptr =
                        reinterpret_cast<
                        const uint8_t*>(data);

                    for (size_t i = 0;
                        i < indexAccessor.count;
                        i++)
                    {
                        indices[i] = ptr[i];
                    }

                    break;
                }
                }
            }

            auto newMesh =
                std::make_shared<Mesh>(vertices, indices);
            
            meshes.push_back(newMesh);
        }
    }
    std::vector<Skeleton> skeletons;

    for (const auto& skin : gltf.skins)
    {
        Skeleton skel;

        // joints
        for (int nodeIndex : skin.joints)
        {
            const auto& node = gltf.nodes[nodeIndex];

            Bone bone;
            bone.name = node.name;
            skel.boneMap[bone.name] = skel.bones.size();

            skel.bones.push_back(bone);
        }

        // inverse bind matrices
        if (skin.inverseBindMatrices >= 0)
        {
            const auto& accessor = gltf.accessors[skin.inverseBindMatrices];
            const float* data = reinterpret_cast<const float*>(
                getBufferData(gltf, accessor));

            for (size_t i = 0; i < skel.bones.size(); i++)
            {
                skel.bones[i].inverseBindMatrix =
                    glm::make_mat4(&data[i * 16]);
            }
        }

        skeletons.push_back(skel);
    }

    std::vector<AnimationClip> animations;
    for (const auto& anim : gltf.animations)
    {
        AnimationClip clip;

        for (const auto& channel : anim.channels)
        {
            const auto& sampler = anim.samplers[channel.sampler];
            const auto& inputAcc = gltf.accessors[sampler.input];
            const auto& outputAcc = gltf.accessors[sampler.output];

            const float* times =
                reinterpret_cast<const float*>(
                    getBufferData(gltf, inputAcc));

            const float* values =
                reinterpret_cast<const float*>(
                    getBufferData(gltf, outputAcc));

            int boneIndex = channel.target_node;

            BoneAnimation boneAnim;

            size_t keyCount = inputAcc.count;

            for (size_t i = 0; i < keyCount; i++)
            {
                KeyFrame kf;
                kf.time = times[i];

                if (sampler.interpolation == "LINEAR")
                {
                    kf.transform.position = { values[i * 3 + 0], values[i * 3 + 1], values[i * 3 + 2] };
                }

                boneAnim.keys.push_back(kf);
            }

            clip.channels[boneIndex] = boneAnim;
        }

        animations.push_back(clip);
    }
    auto model = std::make_shared<Model>(meshes);
    model->bounds.min =
        glm::vec3(std::numeric_limits<float>::max());

    model->bounds.max =
        glm::vec3(-std::numeric_limits<float>::max());

    model->skeletons = skeletons;
    model->animations = animations;
    return model;
}
