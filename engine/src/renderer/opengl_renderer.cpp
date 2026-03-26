#include "positron/renderer/opengl_renderer.h"
#include "positron/core/log.h"

#include <cmath>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ranges>

#undef SHADOW_MAP_SIZE
#define SHADOW_MAP_SIZE 4096

#undef MAX_SHADOW_MAPS
#define MAX_SHADOW_MAPS 4

namespace Positron {
    static constexpr int MAX_LIGHTS = 16;

    static constexpr auto DEPTH_VERT_SRC = R"(
        #version 460 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 uLightSpaceMVP;

        void main() {
            gl_Position = uLightSpaceMVP * vec4(aPos, 1.0);
        }
    )";

    static constexpr auto DEPTH_FRAG_SRC = R"(
        #version 460 core
        void main() {}
    )";

    static constexpr auto FALLBACK_VERT_SRC = R"(
        #version 460 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec3 aNormal;
        layout (location = 3) in vec2 aUV;

        out vec3 vColor;
        out vec2 vUV;
        out vec3 vWorldPos;
        out vec3 vNormal;

        uniform mat4  uMVP;
        uniform mat4  uModel;
        uniform mat3  uNormalMatrix;
        uniform float uTiling;

        void main() {
            vColor      = aColor;
            vUV         = aUV * uTiling;
            vWorldPos   = vec3(uModel * vec4(aPos, 1.0));
            vNormal     = normalize(uNormalMatrix * aNormal);
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )";

    static constexpr auto FALLBACK_FRAG_SRC = R"(
        #version 460 core

        #define MAX_LIGHTS      16
        #define MAX_SHADOW_MAPS 4
        #define LIGHT_DIRECTIONAL 0
        #define LIGHT_POINT       1
        #define LIGHT_SPOT        2

        struct Light {
            int   type;
            vec3  position;
            vec3  direction;
            vec3  color;
            float intensity;
            float range;
            float attenLinear;
            float attenQuad;
            float innerAngleCos;
            float outerAngleCos;
            bool  castShadows;
            int   shadowMapIndex;
            mat4  lightSpaceMatrix;
        };

        in vec3 vColor;
        in vec2 vUV;
        in vec3 vWorldPos;
        in vec3 vNormal;

        out vec4 fragColor;

        uniform sampler2D uAlbedo;
        uniform int       uHasTexture;
        uniform vec4      uTint;
        uniform vec3      uCameraPos;

        uniform vec3  uAmbientColor;
        uniform float uAmbientIntensity;
        uniform int   uLightCount;
        uniform Light uLights[MAX_LIGHTS];

        uniform sampler2D uShadowMaps[MAX_SHADOW_MAPS];

        float shadowFactor(int smIndex, mat4 lightSpaceMat) {
            vec4  fragLS   = lightSpaceMat * vec4(vWorldPos, 1.0);
            vec3  proj     = fragLS.xyz / fragLS.w * 0.5 + 0.5;

            if (proj.z > 1.0)
                return 0.0;

            vec3  toLight  = normalize(-vec3(lightSpaceMat[0][2], lightSpaceMat[1][2], lightSpaceMat[2][2]));
            float cosTheta = clamp(dot(normalize(vNormal), toLight), 0.0, 1.0);
            float bias     = max(0.002, 0.008 * (1.0 - cosTheta));

            float shadow    = 0.0;
            vec2  texelSize = 1.0 / textureSize(uShadowMaps[smIndex], 0);

            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    float pcfDepth = texture(uShadowMaps[smIndex], proj.xy + vec2(x, y) * texelSize).r;
                    shadow += (proj.z - bias > pcfDepth) ? 1.0 : 0.0;
                }
            }
            shadow /= 9.0;
            return shadow * 0.75;
        }

        float attenuation(Light l, float dist) {
            if (l.type == LIGHT_DIRECTIONAL) return 1.0;
            float a = 1.0 / (1.0 + l.attenLinear * dist + l.attenQuad * dist * dist);
            return clamp(a * (1.0 - dist / l.range), 0.0, 1.0);
        }

        vec3 calcLight(Light l, vec3 normal, vec3 viewDir, vec3 albedo) {
            vec3  toLight  = l.type == LIGHT_DIRECTIONAL ? normalize(-l.direction) : normalize(l.position - vWorldPos);
            float dist     = l.type == LIGHT_DIRECTIONAL ? 0.0 : length(l.position - vWorldPos);
            float atten    = attenuation(l, dist);

            float diff     = max(dot(normal, toLight), 0.0);
            vec3  halfVec  = normalize(toLight + viewDir);
            float spec     = pow(max(dot(normal, halfVec), 0.0), 32.0);

            float spotMask = 1.0;
            if (l.type == LIGHT_SPOT) {
                float theta  = dot(toLight, normalize(-l.direction));
                float eps    = l.innerAngleCos - l.outerAngleCos;
                spotMask     = clamp((theta - l.outerAngleCos) / eps, 0.0, 1.0);
            }

            float shadow = 0.0;
            if (l.castShadows && l.shadowMapIndex >= 0)
                shadow = shadowFactor(l.shadowMapIndex, l.lightSpaceMatrix);

            vec3 diffuse  = diff * l.color * l.intensity * albedo;
            vec3 specular = spec * l.color * l.intensity * 0.3;
            return (diffuse + specular) * atten * spotMask * (1.0 - shadow);
        }

        void main() {
            vec4 base = uHasTexture == 1
                ? texture(uAlbedo, vUV)
                : vec4(vColor, 1.0);
            base *= uTint;

            vec3 albedo  = base.rgb;
            vec3 normal  = normalize(vNormal);
            vec3 viewDir = normalize(uCameraPos - vWorldPos);

            vec3 ambient = uAmbientColor * uAmbientIntensity * albedo;
            vec3 result  = ambient;

            for (int i = 0; i < min(uLightCount, MAX_LIGHTS); i++)
                result += calcLight(uLights[i], normal, viewDir, albedo);

            fragColor = vec4(result, base.a);
        }
    )";

    static glm::mat4 buildLightSpaceMatrix(const SceneLight                                      &l,
                                           const std::vector<std::pair<glm::mat4, const Mesh *>> &drawList) {
        glm::mat4 proj;
        glm::mat4 view;

        if (l.type == LightType::Directional) {
            glm::vec3 dir = glm::normalize(l.direction);
            glm::vec3 up  = (std::abs(dir.y) > 0.99f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
            view          = glm::lookAt(glm::vec3(0), dir, up);

            glm::vec3 mn(1e9f), mx(-1e9f);
            for (const auto &model: drawList | std::views::keys) {
                for (float x: {-1.f, 1.f})
                    for (float y: {-1.f, 1.f})
                        for (float z: {-1.f, 1.f}) {
                            glm::vec4 worldPt = model * glm::vec4(x, y, z, 1.f);
                            glm::vec4 lightPt = view * worldPt;
                            mn                = glm::min(mn, glm::vec3(lightPt));
                            mx                = glm::max(mx, glm::vec3(lightPt));
                        }
            }

            constexpr float margin = 1.5f;
            mn -= margin;
            mx += margin;

            proj = glm::ortho(mn.x, mx.x, mn.y, mx.y, 0.f, mx.z - mn.z);

            auto centre = glm::vec3(glm::inverse(view) * glm::vec4((mn + mx) * 0.5f, 1.f));
            view        = glm::lookAt(centre - dir * (mx.z - mn.z) * 0.5f, centre, up);
            proj        = glm::ortho(mn.x, mx.x, mn.y, mx.y, 0.f, (mx.z - mn.z) + margin * 2.f);

        } else {
            proj = glm::perspective(glm::radians(90.f), 1.f, 0.1f, l.range);
            view = glm::lookAt(l.position, l.position + l.direction, glm::vec3(0.f, 1.f, 0.f));
        }

        return proj * view;
    }

    void OpenGLRenderer::initShadowMaps() {
        for (int i = 0; i < MAX_SHADOW_MAPS; ++i) {
            glGenFramebuffers(1, &shadowFBOs_[i]);

            glGenTextures(1, &shadowTexs_[i]);
            glBindTexture(GL_TEXTURE_2D, shadowTexs_[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0,
                         GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            constexpr float borderColor[] = {1.f, 1.f, 1.f, 1.f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOs_[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexs_[i], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    void OpenGLRenderer::shutdownShadowMaps() const {
        glDeleteFramebuffers(MAX_SHADOW_MAPS, shadowFBOs_.data());
        glDeleteTextures(MAX_SHADOW_MAPS, shadowTexs_.data());
    }

    bool OpenGLRenderer::init() {
        if (!GLContext::init())
            return false;

        std::cout << "Positron Engine\n"
                  << "  " << GLContext::getRenderer() << " (" << GLContext::getVendor() << ")\n"
                  << "  OpenGL " << GLContext::getVersion() << " | GLSL " << GLContext::getGLSLVersion() << "\n";

        defaultShader_ = GLShader::fromSource(FALLBACK_VERT_SRC, FALLBACK_FRAG_SRC);
        depthShader_   = GLShader::fromSource(DEPTH_VERT_SRC, DEPTH_FRAG_SRC);

        initShadowMaps();

        glEnable(GL_DEPTH_TEST);
        return true;
    }

    void OpenGLRenderer::shutdown() {
        shutdownShadowMaps();
        delete defaultShader_;
        delete depthShader_;
    }

    void OpenGLRenderer::begin() {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::end() {}

    void OpenGLRenderer::shadowPass(const LightData                                       &lights,
                                    const std::vector<std::pair<glm::mat4, const Mesh *>> &drawList) {
        GLint savedVP[4];
        glGetIntegerv(GL_VIEWPORT, savedVP);

        int slotIndex = 0;

        for (auto &sl: const_cast<LightData &>(lights).lights) {
            if (!sl.castShadows || sl.type == LightType::Point)
                continue;
            if (slotIndex >= MAX_SHADOW_MAPS)
                break;

            sl.lightSpaceMatrix = buildLightSpaceMatrix(sl, drawList);
            sl.shadowMapIndex   = slotIndex;

            glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
            glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOs_[slotIndex]);
            glClear(GL_DEPTH_BUFFER_BIT);

            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(2.f, 4.f);

            depthShader_->bind();
            for (const auto &[model, mesh]: drawList) {
                if (!mesh)
                    continue;
                const glm::mat4 lsMVP = sl.lightSpaceMatrix * model;
                depthShader_->setMat4("uLightSpaceMVP", glm::value_ptr(lsMVP));
                mesh->bind();
                glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr);
                mesh->unbind();
            }
            depthShader_->unbind();

            glDisable(GL_POLYGON_OFFSET_FILL);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            ++slotIndex;
        }

        glViewport(savedVP[0], savedVP[1], savedVP[2], savedVP[3]);
    }

    void OpenGLRenderer::drawMesh(const Mesh *mesh, const glm::mat4 &transform, const MaterialComponent &material,
                                  const CameraData &camera, const LightData &lights) {
        if (!mesh)
            return;

        GLShader *sh = customShader_ ? customShader_ : defaultShader_;

        const glm::mat4 mvp          = camera.projection * camera.view * transform;
        const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
        const glm::vec3 camPos       = glm::vec3(glm::inverse(camera.view)[3]);

        sh->bind();
        sh->setMat4("uMVP", glm::value_ptr(mvp));
        sh->setMat4("uModel", glm::value_ptr(transform));
        sh->setMat3("uNormalMatrix", glm::value_ptr(normalMatrix));
        sh->setVec4("uTint", material.tint.r, material.tint.g, material.tint.b, material.tint.a);
        sh->setFloat("uTiling", material.tiling);
        sh->setVec3("uCameraPos", camPos.x, camPos.y, camPos.z);
        sh->setVec3("uAmbientColor", lights.ambientColor.r, lights.ambientColor.g, lights.ambientColor.b);
        sh->setFloat("uAmbientIntensity", lights.ambientIntensity);

        for (int i = 0; i < MAX_SHADOW_MAPS; ++i) {
            const int texUnit = 1 + i;
            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_2D, shadowTexs_[i]);
            sh->setInt("uShadowMaps[" + std::to_string(i) + "]", texUnit);
        }

        const int lightCount = static_cast<int>(std::min(lights.lights.size(), static_cast<size_t>(MAX_LIGHTS)));
        sh->setInt("uLightCount", lightCount);

        for (int i = 0; i < lightCount; ++i) {
            const auto &l    = lights.lights[i];
            const auto  base = "uLights[" + std::to_string(i) + "].";
            sh->setInt(base + "type", static_cast<int>(l.type));
            sh->setVec3(base + "position", l.position.x, l.position.y, l.position.z);
            sh->setVec3(base + "direction", l.direction.x, l.direction.y, l.direction.z);
            sh->setVec3(base + "color", l.color.x, l.color.y, l.color.z);
            sh->setFloat(base + "intensity", l.intensity);
            sh->setFloat(base + "range", l.range);
            sh->setFloat(base + "attenLinear", l.attenLinear);
            sh->setFloat(base + "attenQuad", l.attenQuad);
            sh->setFloat(base + "innerAngleCos", l.innerAngleCos);
            sh->setFloat(base + "outerAngleCos", l.outerAngleCos);
            sh->setInt(base + "castShadows", l.castShadows ? 1 : 0);
            sh->setInt(base + "shadowMapIndex", l.shadowMapIndex);
            sh->setMat4(base + "lightSpaceMatrix", glm::value_ptr(l.lightSpaceMatrix));
        }

        int texUnit = 0;
        if (material.albedo) {
            glActiveTexture(GL_TEXTURE0);
            material.albedo->bind(0);
            sh->setInt("uAlbedo", 0);
            sh->setInt("uHasTexture", 1);
        } else {
            sh->setInt("uHasTexture", 0);
        }

        mesh->bind();
        glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr);
        mesh->unbind();

        if (material.albedo)
            material.albedo->unbind();

        for (int i = 0; i < MAX_SHADOW_MAPS; ++i) {
            glActiveTexture(GL_TEXTURE0 + 1 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glActiveTexture(GL_TEXTURE0);

        sh->unbind();
    }

    std::string OpenGLRenderer::getDeviceName() const { return GLContext::getRenderer(); }
} // namespace Positron
