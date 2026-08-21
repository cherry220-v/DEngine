#pragma once

#include <assets/IAsset.hpp>

#include <string>
#include <glad/glad.h>
#include <nlohmann/json.hpp>

class Texture : public IAsset
{
public:
    Texture(int width = 0, int height = 0, int channels = 0, unsigned int ID=0) : width(width), height(height), channels(channels), ID(ID) {}
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& other) noexcept;
    Texture(Texture&& other) noexcept;

    void bind(unsigned int slot = 0) const;
    void unbind() const;

    static Texture getWhiteTexture();

    unsigned int getID() const { return ID; }

    std::string getUUID() { return m_uuid; }
    void setUUID(std::string uuid) { m_uuid = uuid; }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
protected:
    std::string m_uuid;
    std::string m_name;
private:
    unsigned int ID = 0;

    const std::string path;
    bool flip = true;

    int width = 0;
    int height = 0;
    int channels = 0;
};

inline nlohmann::json Serialize(Texture* texture);
inline Texture Deserialize(nlohmann::json j);