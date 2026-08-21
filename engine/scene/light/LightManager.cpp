#include "LightManager.hpp"
void LightManager::addLight(const std::shared_ptr<Light>& light) {
    lights.push_back(light);
}

std::shared_ptr<DirectionalLight> LightManager::getDirectionalLight()
{
    // 1. Проверка на явный NULL
    if (this == nullptr)
    {
        //qDebug() << "[LightManager Error]: getDirectionalLight() called on a nullptr LightManager instance!";
        return nullptr;
    }

    // 2. ЗАЩИТА ОТ ДИКИХ УКАЗАТЕЛЕЙ: Если память забита мусором, 
    // размер вектора будет равен миллиардам или отрицательным числам (при касте)
    size_t vecSize = 0;
    try {
        vecSize = lights.size();
    }
    catch (...) {
        //qDebug() << "[LightManager Critical]: lights vector memory is corrupted!";
        return nullptr;
    }

    // Если размер неадекватно большой (больше разумного лимита), это битая память
    if (vecSize > MAX_LIGHTS * 1000)
    {
        //qDebug() << "[LightManager Error]: Memory corruption detected! Invalid vector size:" << vecSize;
        return nullptr;
    }

    if (vecSize == 0) return nullptr;

    // 3. Безопасный обход
    for (auto& l : lights)
    {
        if (!l) continue;

        if (l->type == LightType::Directional)
        {
            auto dirLight = std::dynamic_pointer_cast<DirectionalLight>(l);
            if (dirLight) return dirLight;
        }
    }

    return nullptr;
}
