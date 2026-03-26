#include "positron/core/application.h"
#include "../../include/positron/ecs/scene/scene.h"
#include "positron/core/event_bus.h"
#include "positron/core/input.h"
#include "positron/core/log.h"
#include "positron/core/window.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

namespace Positron {

Application::Application(ApplicationConfig config, std::unique_ptr<Game> game) :
    window_(std::make_unique<Window>(std::move(config.window))),
    game_(std::move(game)),
    renderer_(Renderer::create(game_->getRenderAPI())) {
    Log::init();
}

void Application::run() {
    if (!window_->init()) {
        std::cerr << "Failed to initialize window\n";
        return;
    }

    Input::init(window_->getHandle());

    if (!renderer_->init()) {
        std::cerr << "Failed to initialize renderer\n";
        return;
    }

    scene_ = std::make_unique<Scene>(renderer_.get());

    game_->window_   = window_.get();
    game_->renderer_ = renderer_.get();
    game_->scene_    = scene_.get();

    // Make Window accessible to systems (e.g. FreecamSystem for cursor grab).
    scene_->world().setResource<Window*>(window_.get());

    EventBus::subscribe<WindowCloseEvent>([this](WindowCloseEvent& e) {
        game_->onEvent(e);
        return e.handled;
    });
    EventBus::subscribe<WindowResizeEvent>([this](WindowResizeEvent& e) {
        game_->onEvent(e);
        return e.handled;
    });
    EventBus::subscribe<KeyPressedEvent>([this](KeyPressedEvent& e) {
        game_->onEvent(e);
        return e.handled;
    });
    EventBus::subscribe<KeyReleasedEvent>([this](KeyReleasedEvent& e) {
        game_->onEvent(e);
        return e.handled;
    });
    EventBus::subscribe<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e) {
        game_->onEvent(e);
        return e.handled;
    });
    EventBus::subscribe<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& e) {
        game_->onEvent(e);
        return e.handled;
    });
    EventBus::subscribe<MouseMovedEvent>([this](MouseMovedEvent& e) {
        game_->onEvent(e);
        return e.handled;
    });
    EventBus::subscribe<MouseScrolledEvent>([this](MouseScrolledEvent& e) {
        game_->onEvent(e);
        return e.handled;
    });

    game_->onInit();

    double lastTime = glfwGetTime();

    while (running_ && !window_->shouldClose()) {
        const double now = glfwGetTime();
        const float  dt  = static_cast<float>(now - lastTime);
        lastTime = now;

        Input::update();
        scene_->world().setResource<float>(dt);   // deltaTime resource
        renderer_->begin();
        game_->onUpdate();
        scene_->update();
        renderer_->end();
        window_->onUpdate();

        if (game_->shouldQuit())
            running_ = false;
    }

    game_->onShutdown();
    scene_->shutdown();
    renderer_->shutdown();
    EventBus::clear();
}

}
