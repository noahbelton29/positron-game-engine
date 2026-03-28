//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 27/03/2026.
//

#pragma once

#include "positron/core/export.h"
#include "positron/ecs/core/entity.h"
#include "positron/ecs/core/world.h"
#include "positron/ecs/scene/scene.h"

#include <boost/pfr.hpp>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

namespace Positron {
    class POSITRON_API SceneSerializer {
    public:
        explicit SceneSerializer(Scene &scene);

        template<typename T>
        void extend(std::function<void(std::ofstream &, const T &)>       saveFn,
                    std::function<void(std::ifstream &, Entity, World &)> loadFn) {
            extensions_.push_back(
                    {[](const World &w, const Entity e) { return w.hasComponent<T>(e); },
                     [saveFn](std::ofstream &f, World &w, const Entity e) { saveFn(f, w.getComponent<T>(e)); },
                     loadFn});
        }

        template<typename T>
        void autoExtend() {
            extend<T>(
                    [](std::ofstream &f, const T &c) {
                        boost::pfr::for_each_field(c, [&](const auto &field) {
                            f.write(reinterpret_cast<const char *>(&field), sizeof(field));
                        });
                    },
                    [](std::ifstream &f, const Entity e, World &w) {
                        T c{};
                        boost::pfr::for_each_field(
                                c, [&](auto &field) { f.read(reinterpret_cast<char *>(&field), sizeof(field)); });
                        w.addComponent<T>(e, c);
                    });
        }

        [[nodiscard]] bool save(const std::string &path) const;
        [[nodiscard]] bool load(const std::string &path) const;

    private:
        struct Extension {
            std::function<bool(World &, Entity)>                  has;
            std::function<void(std::ofstream &, World &, Entity)> save;
            std::function<void(std::ifstream &, Entity, World &)> load;
        };

        Scene                 &scene_;
        std::vector<Extension> extensions_;
    };
} // namespace Positron
