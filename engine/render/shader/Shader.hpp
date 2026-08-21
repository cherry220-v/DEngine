#pragma once

#include <assets/IAsset.hpp>

#include <string>
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class AssetManager;

class Shader : public IAsset
{
public:
    Shader(unsigned int ID = 0) : ID(ID) {}
    ~Shader();

    unsigned int getID() const { return ID; }

    void use();

    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setUInt(const std::string& name, uint32_t value);
    void setFloat(const std::string& name, float value);

    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, const glm::vec4& value);

    void setMat4(const std::string& name, const glm::mat4& mat);

    std::string getUUID() { return m_uuid; }
    void setUUID(std::string uuid) { m_uuid = uuid; }

    unsigned int compileShader(unsigned int type, const std::string& source);
    void linkProgram(unsigned int vertex, unsigned int fragment);

    void checkCompileErrors(unsigned int shader, const std::string& type);

    std::string m_name;
protected:
    std::string m_uuid;

    std::string vertexUuid;
    std::string fragmentUuid;

private:
    unsigned int ID = 0;
};