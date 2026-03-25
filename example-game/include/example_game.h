//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
//

#pragma once

#include <positron/core/event.h>
#include <positron/core/event_bus.h>
#include <positron/core/game.h>
#include <positron/renderer/shader.h>

#include <memory>

POSITRON_GAME_EVENT(PlayerDiedEvent, "PlayerDied");
POSITRON_GAME_EVENT_DATA(EnemyDiedEvent, "EnemyDied", (const int id), { enemyId = id; }, int enemyId;);

class ExampleGame : public Positron::Game {
public:
    void onInit() override;
    void onUpdate() override;
    void onShutdown() override;
    void onEvent(Positron::Event &e) override;

    [[nodiscard]] Positron::RenderAPI getRenderAPI() const override;

private:
    std::unique_ptr<Positron::Shader> myShader_;
    Positron::EventScope              events_;
};
