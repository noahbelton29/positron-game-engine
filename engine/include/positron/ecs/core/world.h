//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include "../component/component_storage.h"
#include "entity.h"
#include "positron/core/export.h"
#include "positron/ecs/core/system.h"

#include <cassert>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Positron {
    class POSITRON_API World {
    public:
        World()  = default;
        ~World() = default;

        World(const World &)            = delete;
        World &operator=(const World &) = delete;

        Entity createEntity();
        void   destroyEntity(Entity entity);
        bool   isAlive(Entity entity) const;

        template<typename T>
        T &addComponent(Entity entity, T component = {}) {
            return getStorage<T>().add(entity, std::move(component));
        }

        template<typename T>
        void removeComponent(Entity entity) {
            getStorage<T>().remove(entity);
        }

        template<typename T>
        T &getComponent(Entity entity) {
            return getStorage<T>().get(entity);
        }

        template<typename T>
        const T &getComponent(Entity entity) const {
            return getStorageConst<T>().get(entity);
        }

        template<typename T>
        bool hasComponent(const Entity entity) const {
            const auto it = storages_.find(std::type_index(typeid(T)));
            return it != storages_.end() && it->second->has(entity);
        }

        template<typename T>
        void each(std::function<void(Entity, T &)> fn) {
            auto &s = getStorage<T>();
            for (int i = static_cast<int>(s.entities().size()) - 1; i >= 0; --i)
                fn(s.entities()[i], s.data()[i]);
        }

        template<typename T, typename U>
        void each(std::function<void(Entity, T &, U &)> fn) {
            auto &s = getStorage<T>();
            for (int i = static_cast<int>(s.entities().size()) - 1; i >= 0; --i) {
                if (const Entity entity = s.entities()[i]; hasComponent<U>(entity))
                    fn(entity, s.data()[i], getStorage<U>().get(entity));
            }
        }

        template<typename T, typename U, typename V>
        void each(std::function<void(Entity, T &, U &, V &)> fn) {
            auto &s = getStorage<T>();
            for (int i = static_cast<int>(s.entities().size()) - 1; i >= 0; --i) {
                const Entity entity = s.entities()[i];
                if (hasComponent<U>(entity) && hasComponent<V>(entity))
                    fn(entity, s.data()[i], getStorage<U>().get(entity), getStorage<V>().get(entity));
            }
        }

        template<typename T, typename... Args>
        T *addSystem(Args &&...args) {
            auto sys = std::make_unique<T>(std::forward<Args>(args)...);
            T   *ptr = sys.get();
            ptr->onInit(*this);
            systems_.push_back(std::move(sys));
            return ptr;
        }

        template<typename T>
        T *getSystem() {
            for (auto &s: systems_)
                if (auto *p = dynamic_cast<T *>(s.get()))
                    return p;
            return nullptr;
        }

        void update();
        void shutdown();

        template<typename T>
        void setResource(T value) {
            resources_[std::type_index(typeid(T))] = std::make_unique<ResourceBox<T>>(std::move(value));
        }

        template<typename T>
        T &getResource() {
            const auto it = resources_.find(std::type_index(typeid(T)));
            assert(it != resources_.end() && "Resource not registered on World");
            return static_cast<ResourceBox<T> *>(it->second.get())->value;
        }

        template<typename T>
        bool hasResource() const {
            return resources_.contains(std::type_index(typeid(T)));
        }

    private:
        template<typename T>
        ComponentStorage<T> &getStorage() {
            const auto key = std::type_index(typeid(T));
            const auto it  = storages_.find(key);
            if (it == storages_.end()) {
                auto  s        = std::make_unique<ComponentStorage<T>>();
                auto *p        = s.get();
                storages_[key] = std::move(s);
                return *p;
            }
            return *static_cast<ComponentStorage<T> *>(it->second.get());
        }

        template<typename T>
        const ComponentStorage<T> &getStorageConst() const {
            const auto it = storages_.find(std::type_index(typeid(T)));
            assert(it != storages_.end() && "Component storage not found");
            return *static_cast<const ComponentStorage<T> *>(it->second.get());
        }

        struct IResource {
            virtual ~IResource() = default;
        };

        template<typename T>
        struct ResourceBox : IResource {
            T value;
            explicit ResourceBox(T v) : value(std::move(v)) {}
        };

        Entity                                                                  nextEntity_ = 1;
        std::unordered_set<Entity>                                              alive_;
        std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> storages_;
        std::vector<std::unique_ptr<System>>                                   systems_;
        std::unordered_map<std::type_index, std::unique_ptr<IResource>>         resources_;
    };
} // namespace Positron
