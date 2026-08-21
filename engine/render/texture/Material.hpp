#pragma once

#include <assets/IAsset.hpp>

#include <memory>
#include <glm/vec4.hpp>
#include <string>

class Texture;

class Shader;

class Material : public IAsset
{
public:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> texture;
    std::string m_name;

    glm::vec4 color = glm::vec4(1.0f);

    Material() = default;

    std::string getUUID() { return m_uuid; }
    void setUUID(std::string uuid) { m_uuid = uuid; }

    void bind();

    float shininess = 32.0f;
    float specularStrength = 0.5f;
protected:
    std::string m_uuid;
};