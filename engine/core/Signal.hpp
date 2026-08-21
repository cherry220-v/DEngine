#pragma once

#include <functional>
#include <vector>
#include <memory>

template<typename... Args>
class Signal
{
public:
    using Callback = std::function<void(Args...)>;

    void connect(Callback cb)
    {
        m_callbacks.push_back(std::move(cb));
    }

    void emit(Args... args)
    {
        for (auto& cb : m_callbacks)
            cb(args...);
    }

private:
    std::vector<Callback> m_callbacks;
};