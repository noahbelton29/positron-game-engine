//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include <positron/ecs/component/components.h>
#include <positron/ecs/core/world.h>
#include <positron/core/input.h>
#include <positron/core/window.h>

#include "components/camera_component.h"
#include "systems/camera_system.h"

void CameraSystem::onInit(Positron::World &world) {
    if (world.hasResource<Positron::Window *>())
        world.getResource<Positron::Window *>()->setMouseGrabbed(true);
}

void CameraSystem::onUpdate(Positron::World &world) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    const float aspect = (vp[2] > 0 && vp[3] > 0) ? static_cast<float>(vp[2]) / static_cast<float>(vp[3]) : 16.f / 9.f;

    float dt = 1.f / 60.f;
    if (world.hasResource<float>())
        dt = world.getResource<float>();

    const double mx = Positron::Input::getMouseX();
    const double my = Positron::Input::getMouseY();

    float yawDelta   = 0.f;
    float pitchDelta = 0.f;

    if (!firstFrame_) {
        yawDelta   = static_cast<float>(mx - lastMouseX_) * lookSpeed;
        pitchDelta = static_cast<float>(my - lastMouseY_) * lookSpeed;
    }
    firstFrame_ = false;
    lastMouseX_ = mx;
    lastMouseY_ = my;

    CameraData data;
    bool       found = false;

    world.each<Positron::TransformComponent, CameraComponent>(
            [&](Positron::Entity /*entity*/, Positron::TransformComponent &transform, const CameraComponent &camera) {
                if (found || !camera.active)
                    return;

                transform.rotation.y -= yawDelta;
                transform.rotation.x -= pitchDelta;
                transform.rotation.x = std::clamp(transform.rotation.x, -89.f, 89.f);

                const glm::mat4 rotY = glm::rotate(glm::mat4(1.f), glm::radians(transform.rotation.y), {0.f, 1.f, 0.f});
                const glm::mat4 rotX = glm::rotate(glm::mat4(1.f), glm::radians(transform.rotation.x), {1.f, 0.f, 0.f});
                const auto      forward     = glm::vec3(rotY * rotX * glm::vec4(0.f, 0.f, -1.f, 0.f));
                const auto      right       = glm::vec3(rotY * glm::vec4(1.f, 0.f, 0.f, 0.f));
                const auto      camUp       = glm::vec3(rotY * rotX * glm::vec4(0.f, 1.f, 0.f, 0.f));
                constexpr glm::vec3 worldUp = {0.f, 1.f, 0.f};

                const float speed = moveSpeed * dt;
                if (Positron::Input::isKeyDown(Positron::Key::W))
                    transform.position += forward * speed;
                if (Positron::Input::isKeyDown(Positron::Key::S))
                    transform.position -= forward * speed;
                if (Positron::Input::isKeyDown(Positron::Key::D))
                    transform.position += right * speed;
                if (Positron::Input::isKeyDown(Positron::Key::A))
                    transform.position -= right * speed;
                if (Positron::Input::isKeyDown(Positron::Key::E))
                    transform.position += worldUp * speed;
                if (Positron::Input::isKeyDown(Positron::Key::Q))
                    transform.position -= worldUp * speed;

                data.view       = glm::lookAt(transform.position, transform.position + forward, camUp);
                data.projection = glm::perspective(glm::radians(camera.fov), aspect, camera.nearPlane, camera.farPlane);
                found           = true;
            });

    if (!found) {
        data.view       = glm::translate(glm::mat4(1.f), {0.f, 0.f, -3.f});
        data.projection = glm::perspective(glm::radians(45.f), aspect, 0.1f, 100.f);
    }

    world.setResource<CameraData>(data);
}
