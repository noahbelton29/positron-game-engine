//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include "positron/ecs/core/entity.h"

#include <cassert>
#include <unordered_map>
#include <vector>

namespace Positron {
    struct IComponentStorage {
        virtual ~IComponentStorage()                   = default;
        virtual void               remove(Entity e)    = 0;
        [[nodiscard]] virtual bool has(Entity e) const = 0;
    };

    template<typename T>
    class ComponentStorage final : public IComponentStorage {
    public:
        T &add(const Entity entity, T component) {
            if (const auto it = index_.find(entity); it != index_.end()) {
                data_[it->second] = std::move(component);
                return data_[it->second];
            }
            index_[entity] = static_cast<uint32_t>(data_.size());
            entities_.push_back(entity);
            data_.push_back(std::move(component));
            return data_.back();
        }

        void remove(const Entity entity) override {
            const auto it = index_.find(entity);
            if (it == index_.end())
                return;

            const uint32_t idx  = it->second;
            const uint32_t last = static_cast<uint32_t>(data_.size()) - 1;

            if (idx != last) {
                data_[idx]             = std::move(data_[last]);
                entities_[idx]         = entities_[last];
                index_[entities_[idx]] = idx;
            }

            data_.pop_back();
            entities_.pop_back();
            index_.erase(it);
        }

        bool has(const Entity entity) const override { return index_.contains(entity); }

        T &get(const Entity entity) {
            assert(has(entity) && "Entity does not have this component");
            return data_[index_.at(entity)];
        }

        const T &get(const Entity entity) const {
            assert(has(entity) && "Entity does not have this component");
            return data_[index_.at(entity)];
        }

        std::vector<T>      &data() { return data_; }
        std::vector<Entity> &entities() { return entities_; }

    private:
        std::unordered_map<Entity, uint32_t> index_;
        std::vector<T>                       data_;
        std::vector<Entity>                  entities_;
    };
} // namespace Positron
