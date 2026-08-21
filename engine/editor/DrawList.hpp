#pragma once

#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "DrawTypes.h"
#include <render/texture/Texture.hpp>
#include <render/font/Font.hpp>

class DrawList
{
public:

    // =========================
    // CORE BUFFERS
    // =========================
    std::vector<DrawVert> vtxBuffer;
    std::vector<DrawIdx>  idxBuffer;
    std::vector<DrawCmd>  cmdBuffer;

    // =========================
    // STATE STACKS
    // =========================
    std::vector<Rect>        _ClipRectStack;
    std::vector<TextureRef>  _TextureStack;

    // =========================
    // WRITE POINTERS
    // =========================
    DrawVert* _VtxWritePtr = nullptr;
    DrawIdx* _IdxWritePtr = nullptr;
    DrawIdx   _VtxCurrentIdx = 0;

    // =========================
    // CURRENT DRAW STATE
    // =========================
    struct DrawState
    {
        Rect clipRect;
        uint32_t textureId = 0;

        DrawSpace space = DrawSpace::Screen;
        float sortKey = 0.0f;

        bool depthTest = false;
        bool writeDepth = false;
        bool pickable = false;
    };

    DrawState _state;

public:

    DrawList() = default;
    ~DrawList() = default;

    // =========================================================
    // STATE MANAGEMENT
    // =========================================================

    void PushClipRect(const glm::vec2& min, const glm::vec2& max, bool intersect = false);
    void PopClipRect();

    void PushTexture(TextureRef tex);
    void PopTexture();

    // =========================================================
    // INTERNAL BATCH CONTROL
    // =========================================================

    inline void Clear()
    {
        vtxBuffer.clear();
        idxBuffer.clear();
        cmdBuffer.clear();

        _ClipRectStack.clear();
        _TextureStack.clear();

        _VtxWritePtr = nullptr;
        _IdxWritePtr = nullptr;
        _VtxCurrentIdx = 0;
    }

    inline void FlushIfNeeded()
    {
        if (cmdBuffer.empty())
            return;

        auto& prev = cmdBuffer.back();

        if (prev.elemCount == 0)
            return;

        DrawCmd cmd;
        cmd.idxOffset = (uint32_t)idxBuffer.size();
        cmd.vtxOffset = (uint32_t)vtxBuffer.size();

        cmd.textureId = _state.textureId;
        cmd.clipRect = _state.clipRect;
        cmd.space = _state.space;
        cmd.sortKey = _state.sortKey;

        cmdBuffer.push_back(cmd);
    }

    void BeginCmdIfNeeded();

    void UpdateStateIfNeeded();

    // =========================================================
    // PRIMITIVE CORE
    // =========================================================

    inline void PrimReserve(uint32_t idxCount, uint32_t vtxCount)
    {
        if (idxCount == 0 && vtxCount == 0)
            return;

        size_t vtxOffset = vtxBuffer.size();
        size_t idxOffset = idxBuffer.size();

        vtxBuffer.resize(vtxOffset + vtxCount);
        idxBuffer.resize(idxOffset + idxCount);

        _VtxWritePtr = &vtxBuffer[vtxOffset];
        _IdxWritePtr = &idxBuffer[idxOffset];

        _VtxCurrentIdx = (DrawIdx)vtxOffset;

        if (cmdBuffer.empty())
        {
            cmdBuffer.emplace_back();
        }

        cmdBuffer.back().elemCount += idxCount;
    }

    inline void PrimWriteVtx(const glm::vec2& pos, const glm::vec2& uv, U32 col, float depth=0.0f)
    {
        _VtxWritePtr->pos[0] = pos.x;
        _VtxWritePtr->pos[1] = pos.y;
        _VtxWritePtr->uv[0] = uv.x;
        _VtxWritePtr->uv[1] = uv.y;
        _VtxWritePtr->col = col;
        _VtxWritePtr->depth = depth;

        ++_VtxWritePtr;
        ++_VtxCurrentIdx;
    }

    inline void PrimWriteIdx(DrawIdx idx)
    {
        *_IdxWritePtr = idx;
        ++_IdxWritePtr;
    }

    inline void PrimVtx(const glm::vec2& pos, const glm::vec2& uv, U32 col)
    {
        PrimWriteIdx(_VtxCurrentIdx);
        PrimWriteVtx(pos, uv, col);
    }

    // =========================================================
    // PRIMITIVES (ENGINE SAFE)
    // =========================================================

    void AddLine(const glm::vec2& p1, const glm::vec2& p2, U32 col, float thickness = 1.0f);
    void AddRect(const glm::vec2& p_min, const glm::vec2& p_max, U32 col, float thickness = 1.0f);
    void AddRectFilled(const glm::vec2& p_min, const glm::vec2& p_max, U32 col);

    void AddQuad(const glm::vec2& p1, const glm::vec2& p2,
        const glm::vec2& p3, const glm::vec2& p4,
        U32 col, float thickness = 1.0f);

    void AddQuadFilled(const glm::vec2& p1, const glm::vec2& p2,
        const glm::vec2& p3, const glm::vec2& p4,
        U32 col);

    void AddRectFilledMultiColor(const glm::vec2& p_min, const glm::vec2& p_max,
        U32 c1, U32 c2, U32 c3, U32 c4);

    // =========================================================
    // TEXT / IMAGE
    // =========================================================

    void AddText(const glm::vec2& pos, U32 col, const char* text);
    void AddText(Font* font, float size, const glm::vec2& pos, U32 col, const char* text);

    void AddImage(TextureRef tex, const glm::vec2& p_min, const glm::vec2& p_max, U32 col = 0xFFFFFFFF);

    // =========================================================
    // INTERNAL HELPERS
    // =========================================================

private:

    void NewDrawCmd();
    void ApplyStateIfChanged();
};