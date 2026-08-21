#pragma once

#include "RenderResource.hpp"
#include <editor/gizmo/ImGuizmo.h>

#include <unordered_map>
#include <memory>
#include <stdexcept>


class RenderGraphStorage
{
public:
    inline int getCurrentOperation() const { return static_cast<int>(m_CurrentOp); }
    inline int getCurrentMode() const { return static_cast<int>(m_CurrentMode); }

    void setCurrentOperation(GUIZMO_NAMESPACE::OPERATION op) { m_CurrentOp = op; }
    void setCurrentMode(GUIZMO_NAMESPACE::MODE mode) { m_CurrentMode = mode; }

    RenderHandle create(const std::string& name)
    {
        RenderHandle h;
        h = nextId++;

        nameToHandle[name] = h;

        return h;
    }

    template<typename T>
    void set(RenderHandle h, std::shared_ptr<T> value)
    {
        RenderResource res;
        data[h] = res;
        data[h].data = value;
        data[h].type = typeid(T);
    }

    template<typename T>
    std::shared_ptr<T> get(RenderHandle h)
    {
        auto& r = data.at(h);

        return std::static_pointer_cast<T>(r.data);
    }

    void publish(const std::string& name, RenderHandle tex)
    {
        nameToHandle[name] = std::move(tex);
    }

    RenderHandle* import(const std::string& name)
    {
        auto it = nameToHandle.find(name);
        if (it == nameToHandle.end())
            return nullptr;

        return &it->second;
    }

    void reset() { nameToHandle.clear(); data.clear(); nextId = 1; }

private:
    GUIZMO_NAMESPACE::OPERATION m_CurrentOp = GUIZMO_NAMESPACE::OPERATION::TRANSLATE;
    GUIZMO_NAMESPACE::MODE      m_CurrentMode = GUIZMO_NAMESPACE::MODE::LOCAL;

    std::unordered_map<std::string, RenderHandle> nameToHandle;
    std::unordered_map<RenderHandle, RenderResource> data;
    RenderHandle nextId = 1;
};