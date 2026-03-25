#pragma once

#include "positron/renderer/renderer.h"

namespace Positron {
    class Game {
    public:
        virtual ~Game() = default;

        virtual void onInit()     = 0;
        virtual void onUpdate()   = 0;
        virtual void onShutdown() = 0;

        [[nodiscard]] virtual RenderAPI getRenderAPI() const { return RenderAPI::OpenGL; }

        [[nodiscard]] bool shouldQuit() const { return shouldQuit_; }

    protected:
        void quit() { shouldQuit_ = true; }

    private:
        bool shouldQuit_ = false;
    };
} // namespace Positron
