#pragma once

#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>

inline std::ostream& operator<<(std::ostream& os, const glm::mat4& m) {
    os << "\n";
    for (int i = 0; i < 4; ++i) {
        os << "[ ";
        for (int j = 0; j < 4; ++j) {
            os << std::setw(12) << std::fixed << std::setprecision(5) << m[j][i] << " ";
        }
        os << "]\n";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
    os << std::fixed << std::setprecision(5)
        << "[ " << v.x << ", " << v.y << " ]";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    os << std::fixed << std::setprecision(5)
        << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
    os << std::fixed << std::setprecision(5)
        << "[ " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << " ]";
    return os;
}