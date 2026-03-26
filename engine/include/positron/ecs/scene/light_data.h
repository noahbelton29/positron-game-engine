#pragma once

#include "positron/ecs/component/components.h"
#include <glm/glm.hpp>
#include <vector>

namespace Positron {
    struct SceneLight {
        LightType type;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float     intensity;
        float     range;
        float     attenLinear;
        float     attenQuad;
        float     innerAngleCos;
        float     outerAngleCos;
        bool      castShadows;
        glm::mat4 lightSpaceMatrix = glm::mat4(1.f);
        int       shadowMapIndex   = -1;
    };

    struct LightData {
        std::vector<SceneLight> lights;
        glm::vec3               ambientColor     = {1.f, 1.f, 1.f};
        float                   ambientIntensity = 0.05f;
    };
} // namespace Positron
