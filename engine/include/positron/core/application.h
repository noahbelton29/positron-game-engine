#pragma once

#include "../ecs/scene/scene.h"
#include "positron/core/export.h"
#include "positron/core/game.h"
#include "positron/core/window.h"
#include "positron/renderer/renderer.h"

#include <memory>

namespace Positron {
    struct POSITRON_API ApplicationConfig {
        WindowConfig window;
    };

    class POSITRON_API Application {
    public:
        Application(ApplicationConfig config, std::unique_ptr<Game> game);
        ~Application() = default;

        Application(const Application &)            = delete;
        Application &operator=(const Application &) = delete;

        void run();
        void quit() { running_ = false; }

    private:
        std::unique_ptr<Window>   window_;
        std::unique_ptr<Game>     game_;
        std::unique_ptr<Renderer> renderer_;
        std::unique_ptr<Scene>    scene_;
        bool                      running_ = true;
    };

} // namespace Positron
