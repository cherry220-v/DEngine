#include "DrawList.hpp"

// =========================================================
// INTERNAL: START NEW COMMAND
// =========================================================

void DrawList::NewDrawCmd()
{
    DrawCmd cmd{};
    cmd.elemCount = 0;
    cmd.textureId = _state.textureId;
    cmd.clipRect = _state.clipRect;
    cmd.space = _state.space;
    cmd.sortKey = _state.sortKey;

    cmdBuffer.push_back(cmd);
}

// =========================================================
// STATE UPDATE HELPERS
// =========================================================

void DrawList::ApplyStateIfChanged()
{
    if (cmdBuffer.empty())
        NewDrawCmd();

    DrawCmd& cmd = cmdBuffer.back();

    bool stateMismatch =
        cmd.textureId != _state.textureId ||
        cmd.clipRect.Min != _state.clipRect.Min ||
        cmd.clipRect.Max != _state.clipRect.Max ||
        cmd.space != _state.space;

    if (stateMismatch && cmd.elemCount > 0)
    {
        NewDrawCmd();
    }

    if (cmd.elemCount == 0)
    {
        cmd.textureId = _state.textureId;
        cmd.clipRect = _state.clipRect;
        cmd.space = _state.space;
        cmd.sortKey = _state.sortKey;
    }
}

// =========================================================
// CLIP RECT STACK
// =========================================================

void DrawList::PushClipRect(const glm::vec2& min, const glm::vec2& max, bool intersect)
{
    Rect r(min, max);

    if (intersect && !_ClipRectStack.empty())
    {
        const Rect& cur = _ClipRectStack.back();

        r.Min.x = glm::max(r.Min.x, cur.Min.x);
        r.Min.y = glm::max(r.Min.y, cur.Min.y);
        r.Max.x = glm::min(r.Max.x, cur.Max.x);
        r.Max.y = glm::min(r.Max.y, cur.Max.y);
    }

    _ClipRectStack.push_back(r);

    _state.clipRect = r;
}

void DrawList::PopClipRect()
{
    if (_ClipRectStack.empty())
        return;

    _ClipRectStack.pop_back();

    if (_ClipRectStack.empty())
    {
        _state.clipRect = Rect({ 0,0 }, { 1920,1080 });
    }
    else
    {
        _state.clipRect = _ClipRectStack.back();
    }
}

// =========================================================
// TEXTURE STACK
// =========================================================

void DrawList::PushTexture(TextureRef tex)
{
    _TextureStack.push_back(tex);

    _state.textureId = tex ? tex->getID() : 0;
}

void DrawList::PopTexture()
{
    if (_TextureStack.empty())
        return;

    _TextureStack.pop_back();

    _state.textureId =
        _TextureStack.empty()
        ? 0
        : _TextureStack.back()->getID();
}

// =========================================================
// LINE
// =========================================================

void DrawList::AddLine(const glm::vec2& p1, const glm::vec2& p2, U32 col, float thickness)
{
    if ((col & 0xFF000000) == 0)
        return;

    ApplyStateIfChanged();

    glm::vec2 d = p2 - p1;
    float len2 = d.x * d.x + d.y * d.y;
    if (len2 <= 0.0f)
        return;

    float invLen = 1.0f / sqrtf(len2);
    glm::vec2 n = glm::vec2(-d.y, d.x) * invLen;

    glm::vec2 off = n * (thickness * 0.5f);

    glm::vec2 v0 = p1 + off;
    glm::vec2 v1 = p2 + off;
    glm::vec2 v2 = p2 - off;
    glm::vec2 v3 = p1 - off;

    PrimReserve(6, 4);

    DrawIdx base = _VtxCurrentIdx;

    PrimWriteVtx(v0, { 0,0 }, col);
    PrimWriteVtx(v1, { 0,0 }, col);
    PrimWriteVtx(v2, { 0,0 }, col);
    PrimWriteVtx(v3, { 0,0 }, col);

    PrimWriteIdx(base + 0);
    PrimWriteIdx(base + 1);
    PrimWriteIdx(base + 2);

    PrimWriteIdx(base + 0);
    PrimWriteIdx(base + 2);
    PrimWriteIdx(base + 3);
}

// =========================================================
// RECT (OUTLINE)
// =========================================================

void DrawList::AddRect(const glm::vec2& a, const glm::vec2& b, U32 col, float thickness)
{
    if ((col & 0xFF000000) == 0)
        return;

    ApplyStateIfChanged();

    glm::vec2 p1 = a;
    glm::vec2 p2 = { b.x, a.y };
    glm::vec2 p3 = b;
    glm::vec2 p4 = { a.x, b.y };

    AddLine(p1, p2, col, thickness);
    AddLine(p2, p3, col, thickness);
    AddLine(p3, p4, col, thickness);
    AddLine(p4, p1, col, thickness);
}

// =========================================================
// RECT FILLED
// =========================================================

void DrawList::AddRectFilled(const glm::vec2& a, const glm::vec2& b, U32 col)
{
    if ((col & 0xFF000000) == 0)
        return;

    ApplyStateIfChanged();

    PrimReserve(6, 4);

    DrawIdx base = _VtxCurrentIdx;

    PrimWriteVtx(a, { 0,0 }, col);
    PrimWriteVtx({ b.x,a.y }, { 0,0 }, col);
    PrimWriteVtx(b, { 0,0 }, col);
    PrimWriteVtx({ a.x,b.y }, { 0,0 }, col);

    PrimWriteIdx(base + 0);
    PrimWriteIdx(base + 1);
    PrimWriteIdx(base + 2);

    PrimWriteIdx(base + 0);
    PrimWriteIdx(base + 2);
    PrimWriteIdx(base + 3);
}

// =========================================================
// QUAD
// =========================================================

void DrawList::AddQuad(const glm::vec2& p1, const glm::vec2& p2,
    const glm::vec2& p3, const glm::vec2& p4,
    U32 col, float thickness)
{
    AddLine(p1, p2, col, thickness);
    AddLine(p2, p3, col, thickness);
    AddLine(p3, p4, col, thickness);
    AddLine(p4, p1, col, thickness);
}

// =========================================================
// QUAD FILLED
// =========================================================

void DrawList::AddQuadFilled(const glm::vec2& p1, const glm::vec2& p2,
    const glm::vec2& p3, const glm::vec2& p4,
    U32 col)
{
    if ((col & 0xFF000000) == 0)
        return;

    ApplyStateIfChanged();

    PrimReserve(6, 4);

    DrawIdx base = _VtxCurrentIdx;

    PrimWriteVtx(p1, { 0,0 }, col);
    PrimWriteVtx(p2, { 0,0 }, col);
    PrimWriteVtx(p3, { 0,0 }, col);
    PrimWriteVtx(p4, { 0,0 }, col);

    PrimWriteIdx(base + 0);
    PrimWriteIdx(base + 1);
    PrimWriteIdx(base + 2);

    PrimWriteIdx(base + 0);
    PrimWriteIdx(base + 2);
    PrimWriteIdx(base + 3);
}

// =========================================================
// MULTI COLOR RECT
// =========================================================

void DrawList::AddRectFilledMultiColor(const glm::vec2& a, const glm::vec2& b,
    U32 c1, U32 c2, U32 c3, U32 c4)
{
    if (((c1 | c2 | c3 | c4) & 0xFF000000) == 0)
        return;

    ApplyStateIfChanged();

    PrimReserve(6, 4);

    DrawIdx base = _VtxCurrentIdx;

    PrimWriteVtx(a, { 0,0 }, c1);
    PrimWriteVtx({ b.x,a.y }, { 0,0 }, c2);
    PrimWriteVtx(b, { 0,0 }, c3);
    PrimWriteVtx({ a.x,b.y }, { 0,0 }, c4);

    PrimWriteIdx(base + 0);
    PrimWriteIdx(base + 1);
    PrimWriteIdx(base + 2);

    PrimWriteIdx(base + 0);
    PrimWriteIdx(base + 2);
    PrimWriteIdx(base + 3);
}

// =========================================================
// TEXT (STUB SAFE)
// =========================================================

void DrawList::AddText(const glm::vec2& pos, U32 col, const char* text)
{
    if (!text || (col & 0xFF000000) == 0)
        return;

    ApplyStateIfChanged();

    // placeholder: depends on your font system
}

// =========================================================
// IMAGE (STUB SAFE)
// =========================================================

void DrawList::AddImage(TextureRef tex,
    const glm::vec2& a,
    const glm::vec2& b,
    U32 col)
{
    if (!tex)
        return;

    ApplyStateIfChanged();

    PushTexture(tex);
    AddRectFilled(a, b, col);
    PopTexture();
}