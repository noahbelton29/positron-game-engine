//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/core/event.h"
#include "positron/core/export.h"

#include <functional>
#include <unordered_map>
#include <vector>

namespace Positron {
    class POSITRON_API EventDispatcher {
    public:
        explicit EventDispatcher(Event &event) : event_(event) {}

        template<typename T, typename F>
        bool dispatch(const F &handler) {
            if (event_.getType() == T::getStaticType() && !event_.handled) {
                event_.handled = handler(static_cast<T &>(event_));
                return true;
            }
            return false;
        }

    private:
        Event &event_;
    };

    class POSITRON_API EventBus {
    public:
        using HandlerFn    = std::function<bool(Event &)>;
        using SubscriberID = uint32_t;

        template<typename T>
        static SubscriberID subscribe(std::function<bool(T &)> handler) {
            auto id      = nextID_++;
            auto wrapped = [handler](Event &e) -> bool { return handler(static_cast<T &>(e)); };
            handlers_[T::getStaticType()].push_back({id, std::move(wrapped)});
            return id;
        }

        static void unsubscribe(SubscriberID id);
        static void emit(Event &event);

        template<typename T, typename... Args>
        static void emit(Args &&...args) {
            T event{std::forward<Args>(args)...};
            emit(event);
        }

        static void clear();

    private:
        struct Entry {
            SubscriberID id;
            HandlerFn    fn;
        };

        static std::unordered_map<EventTypeID, std::vector<Entry>> handlers_;
        static SubscriberID                                        nextID_;
    };
    class POSITRON_API EventScope {
    public:
        EventScope() = default;
        ~EventScope() { clear(); }

        EventScope(const EventScope &)            = delete;
        EventScope &operator=(const EventScope &) = delete;
        EventScope(EventScope &&)                 = default;
        EventScope &operator=(EventScope &&)      = default;

        template<typename T>
        void subscribe(std::function<bool(T &)> handler) {
            ids_.push_back(EventBus::subscribe<T>(std::move(handler)));
        }

        void clear() {
            for (const auto id: ids_)
                EventBus::unsubscribe(id);
            ids_.clear();
        }

    private:
        std::vector<EventBus::SubscriberID> ids_;
    };

    class POSITRON_API EventListener {
    public:
        virtual ~EventListener() = default;

    protected:
        template<typename T>
        void listen(std::function<bool(T &)> handler) {
            scope_.subscribe<T>(std::move(handler));
        }

        void stopListening() { scope_.clear(); }

    private:
        EventScope scope_;
    };
} // namespace Positron
