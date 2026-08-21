#pragma once
#include <vector>
#include <unordered_map>

struct DrawGlyph {
    uint32_t Codepoint;     // Unicode код символа (например, 65 для 'A')
    float    AdvanceX;      // На сколько пикселей сдвинуть курсор вправо после этого символа
    float    X0, Y0, X1, Y1;// Границы символа на экране (относительно курсора)
    float    U0, V0, U1, V1;// Текстурные координаты этого символа в общем атласе
};

class Font {
public:
    float                      FontSize;
    void* ContainerAtlas; // Ссылка на родительский атлас шрифтов
    std::unordered_map<uint32_t, DrawGlyph> Glyphs;

    const DrawGlyph* FindGlyph(uint32_t codepoint) const {
        auto it = Glyphs.find(codepoint);
        if (it != Glyphs.end()) return &it->second;

        it = Glyphs.find('?');
        return it != Glyphs.end() ? &it->second : nullptr;
    }
};
