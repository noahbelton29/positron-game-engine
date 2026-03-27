#pragma once

#include "positron/core/export.h"
#include "positron/ecs/core/entity.h"
#include "positron/ecs/core/world.h"
#include "positron/ecs/scene/scene.h"

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

        bool save(const std::string &path) const;
        bool load(const std::string &path) const;

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
