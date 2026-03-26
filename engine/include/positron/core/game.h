#pragma once

#include "../ecs/scene/scene.h"
#include "positron/core/event.h"
#include "positron/renderer/renderer.h"

namespace Positron {
    class Window;

    class Game {
    public:
        virtual ~Game() = default;

        virtual void onInit()     = 0;
        virtual void onUpdate()   = 0;
        virtual void onShutdown() = 0;

        virtual void onEvent(Event &) {}

        [[nodiscard]] virtual RenderAPI getRenderAPI() const { return RenderAPI::OpenGL; }

        [[nodiscard]] bool      shouldQuit() const { return shouldQuit_; }
        [[nodiscard]] Window   *getWindow() const { return window_; }
        [[nodiscard]] Renderer *getRenderer() const { return renderer_; }
        [[nodiscard]] Scene    &getScene() const { return *scene_; }

    protected:
        void quit() { shouldQuit_ = true; }

        friend class Application;

    private:
        bool      shouldQuit_ = false;
        Window   *window_     = nullptr;
        Renderer *renderer_   = nullptr;
        Scene    *scene_      = nullptr;
    };

} // namespace Positron
