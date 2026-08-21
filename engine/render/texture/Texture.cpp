#include "Texture.hpp"
#include "Texture.hpp"

Texture::~Texture()
{
    if (ID != 0)
    {
        glDeleteTextures(1, &ID);
        ID = 0;
    }
}

Texture::Texture(Texture&& other) noexcept
{
    ID = other.ID;
    other.ID = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this == &other)
        return *this;

    glDeleteTextures(1, &ID);

    ID = other.ID;
    other.ID = 0;

    return *this;
}

void Texture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture Texture::getWhiteTexture()
{
    static unsigned int whiteTextureID = 0;

    if (whiteTextureID == 0)
    {
        glGenTextures(1, &whiteTextureID);
        glBindTexture(GL_TEXTURE_2D, whiteTextureID);

        unsigned char whitePixel[] = { 255, 255, 255, 255 };

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    return Texture(1, 1, 4, whiteTextureID);
}
