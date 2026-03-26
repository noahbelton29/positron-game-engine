#include "positron/ecs/systems/render_system.h"
#include "positron/ecs/component/components.h"
#include "positron/ecs/core/world.h"
#include "positron/ecs/scene/camera_data.h"
#include "positron/ecs/scene/light_data.h"
#include "positron/renderer/renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Positron {
    void RenderSystem::onUpdate(World &world) {
        if (!world.hasResource<Renderer *>())
            return;
        auto *renderer = world.getResource<Renderer *>();

        CameraData camera;
        if (world.hasResource<CameraData>())
            camera = world.getResource<CameraData>();

        LightData lights;
        world.each<TransformComponent, LightComponent>(
                [&lights](Entity, const TransformComponent &transform, const LightComponent &light) {
                    const glm::mat4 rot = glm::rotate(glm::mat4(1.f), glm::radians(transform.rotation.y), {0.f, 1.f, 0.f})
                                        * glm::rotate(glm::mat4(1.f), glm::radians(transform.rotation.x), {1.f, 0.f, 0.f})
                                        * glm::rotate(glm::mat4(1.f), glm::radians(transform.rotation.z), {0.f, 0.f, 1.f});
                    const glm::vec3 worldDir = glm::normalize(glm::vec3(rot * glm::vec4(light.direction, 0.f)));

                    SceneLight sl;
                    sl.type          = light.type;
                    sl.position      = transform.position;
                    sl.direction     = worldDir;
                    sl.color         = light.color;
                    sl.intensity     = light.intensity;
                    sl.range         = light.range;
                    sl.attenLinear   = light.attenLinear;
                    sl.attenQuad     = light.attenQuad;
                    sl.innerAngleCos = glm::cos(glm::radians(light.innerAngle));
                    sl.outerAngleCos = glm::cos(glm::radians(light.outerAngle));
                    sl.castShadows   = light.castShadows;
                    lights.lights.push_back(sl);
                });

        std::vector<std::pair<glm::mat4, const Mesh *>> drawList;
        world.each<TransformComponent, MeshRendererComponent>(
                [&drawList](Entity, const TransformComponent &transform, const MeshRendererComponent &mr) {
                    if (mr.visible && mr.mesh)
                        drawList.emplace_back(transform.matrix(), mr.mesh);
                });

        renderer->shadowPass(lights, drawList);

        world.each<TransformComponent, MeshRendererComponent>(
                [renderer, &camera, &lights](Entity, const TransformComponent &transform,
                                             const MeshRendererComponent &mr) {
                    if (mr.visible && mr.mesh)
                        renderer->drawMesh(mr.mesh, transform.matrix(), mr.material, camera, lights);
                });
    }
} // namespace Positron
