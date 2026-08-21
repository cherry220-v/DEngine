#pragma once

#include <render/texture/Texture.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vector>

class IRenderDevice;

struct Line
{
    glm::vec3 a;
    glm::vec3 b;
    glm::vec4 color;
    float thickness = 1.0f;
};

struct Circle
{
    glm::vec3 center;
    glm::vec3 normal;
    float radius;
    glm::vec4 color;
};

struct Cone
{
    glm::vec3 pos;
    glm::vec3 dir;
    float radius;
    float height;
    glm::vec4 color;
};

struct Billboard
{
    glm::vec3 position;
    std::shared_ptr<Texture> texture;
};

struct Arrow
{
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 camPos;
    glm::vec4 color;
    float thickness = 1.0f;
};

class EditorDrawList
{
public:
    void line(const glm::vec3& a, const glm::vec3& b, const glm::vec4& color)
    {
        lines.push_back({ a, b, color });
    }

    void circle(const glm::vec3& c, const glm::vec3& n, float r, const glm::vec4& col)
    {
        circles.push_back({ c, n, r, col });
    }

    void cone(const glm::vec3& p, const glm::vec3& d, float r, float h, const glm::vec4& col)
    {
        cones.push_back({ p, d, r, h, col });
    }

    void billboard(const glm::vec3& pos, std::shared_ptr<Texture> tex)
    {
        billboards.push_back({ pos, tex });
    }

    void arrow(const glm::vec3& a, const glm::vec3& b, const glm::vec3& camPos, const glm::vec4& color, float thickness)
    {
        arrows.push_back({ a, b, camPos, color, thickness });
    }

    void clear()
    {
        lines.clear();
        circles.clear();
        cones.clear();
        billboards.clear();
    }

    const std::vector<Line>& getLines() const { return lines; }
    const std::vector<Circle>& getCircles() const { return circles; }
    const std::vector<Cone>& getCones() const { return cones; }
    const std::vector<Billboard>& getBillboards() const { return billboards; }
    const std::vector<Arrow>& getArrows() const { return arrows; }

private:
    std::vector<Line> lines;
    std::vector<Circle> circles;
    std::vector<Cone> cones;
    std::vector<Billboard> billboards;
    std::vector<Arrow> arrows;
};

void renderEditorDrawList(
    const EditorDrawList& drawList,
    const glm::mat4& view,
    const glm::mat4& proj,
    float viewportW,
    float viewportH,
    IRenderDevice* device);