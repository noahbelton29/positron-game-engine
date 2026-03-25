//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "positron/core/event_bus.h"

#include <algorithm>
#include <ranges>

namespace Positron {
    std::unordered_map<EventTypeID, std::vector<EventBus::Entry>> EventBus::handlers_;
    EventBus::SubscriberID                                        EventBus::nextID_ = 1;

    void EventBus::emit(Event &event) {
        const auto it = handlers_.find(event.getType());
        if (it == handlers_.end())
            return;

        for (auto &[id, fn]: it->second) {
            if (event.handled)
                break;
            fn(event);
        }
    }

    void EventBus::unsubscribe(SubscriberID id) {
        for (auto &entries: handlers_ | std::views::values) {
            if (auto it = std::ranges::remove_if(entries, [id](const Entry &e) { return e.id == id; }).begin();
                it != entries.end()) {
                entries.erase(it, entries.end());
                return;
            }
        }
    }

    void EventBus::clear() { handlers_.clear(); }
} // namespace Positron
