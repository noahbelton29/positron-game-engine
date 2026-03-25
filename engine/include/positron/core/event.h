//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
//

#pragma once

#include "positron/core/export.h"
#include "positron/core/input.h"

#include <string>

namespace Positron {
    using EventTypeID = std::string;

    enum class EventCategory : unsigned int {
        None        = 0,
        Application = 1 << 0,
        Input       = 1 << 1,
        Keyboard    = 1 << 2,
        Mouse       = 1 << 3,
        Game        = 1 << 4,
    };

    inline EventCategory operator|(EventCategory a, EventCategory b) {
        return static_cast<EventCategory>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
    }
    inline bool operator&(EventCategory a, EventCategory b) {
        return (static_cast<unsigned>(a) & static_cast<unsigned>(b)) != 0;
    }

    class POSITRON_API Event {
    public:
        virtual ~Event()                                          = default;
        [[nodiscard]] virtual EventTypeID   getType() const       = 0;
        [[nodiscard]] virtual EventCategory getCategories() const = 0;
        [[nodiscard]] virtual std::string   getName() const       = 0;

        bool handled = false;
    };

#define POSITRON_EVENT_TYPE(typeName)                                                                                  \
    static const Positron::EventTypeID &getStaticType() {                                                              \
        static const Positron::EventTypeID id = typeName;                                                              \
        return id;                                                                                                     \
    }                                                                                                                  \
    [[nodiscard]] Positron::EventTypeID getType() const override { return getStaticType(); }                           \
    [[nodiscard]] std::string           getName() const override { return typeName; }

#define POSITRON_EVENT_CATEGORY(cat)                                                                                   \
    [[nodiscard]] Positron::EventCategory getCategories() const override { return (cat); }

#define POSITRON_GAME_EVENT(ClassName, typeName)                                                                       \
    class ClassName : public Positron::Event {                                                                         \
    public:                                                                                                            \
        POSITRON_EVENT_TYPE(typeName)                                                                                  \
        POSITRON_EVENT_CATEGORY(Positron::EventCategory::Game)                                                         \
    }

#define POSITRON_GAME_EVENT_DATA(ClassName, typeName, ctorParams, ctorBody, ...)                                       \
    class ClassName : public Positron::Event {                                                                         \
    public:                                                                                                            \
        __VA_ARGS__                                                                                                    \
        explicit ClassName ctorParams ctorBody POSITRON_EVENT_TYPE(typeName)                                           \
                POSITRON_EVENT_CATEGORY(Positron::EventCategory::Game)                                                 \
    }

    // application events
    class POSITRON_API WindowCloseEvent : public Event {
    public:
        POSITRON_EVENT_TYPE("WindowClose")
        POSITRON_EVENT_CATEGORY(EventCategory::Application)
    };

    class POSITRON_API WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(const unsigned int width, const unsigned int height) : width_(width), height_(height) {}

        [[nodiscard]] unsigned int getWidth() const { return width_; }
        [[nodiscard]] unsigned int getHeight() const { return height_; }

        POSITRON_EVENT_TYPE("WindowResize")
        POSITRON_EVENT_CATEGORY(EventCategory::Application)

    private:
        unsigned int width_, height_;
    };
    // key events
    class POSITRON_API KeyEvent : public Event {
    public:
        [[nodiscard]] Key getKey() const { return key_; }

    protected:
        explicit KeyEvent(const Key key) : key_(key) {}
        Key key_;
    };

    class POSITRON_API KeyPressedEvent : public KeyEvent {
    public:
        explicit KeyPressedEvent(const Key key) : KeyEvent(key) {}

        POSITRON_EVENT_TYPE("KeyPressed")
        POSITRON_EVENT_CATEGORY(EventCategory::Input | EventCategory::Keyboard)
    };

    class POSITRON_API KeyReleasedEvent : public KeyEvent {
    public:
        explicit KeyReleasedEvent(const Key key) : KeyEvent(key) {}

        POSITRON_EVENT_TYPE("KeyReleased")
        POSITRON_EVENT_CATEGORY(EventCategory::Input | EventCategory::Keyboard)
    };

    // mouse events
    class POSITRON_API MouseButtonEvent : public Event {
    public:
        [[nodiscard]] MouseButton getButton() const { return button_; }

    protected:
        explicit MouseButtonEvent(const MouseButton button) : button_(button) {}
        MouseButton button_;
    };

    class POSITRON_API MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        explicit MouseButtonPressedEvent(const MouseButton button) : MouseButtonEvent(button) {}

        POSITRON_EVENT_TYPE("MouseButtonPressed")
        POSITRON_EVENT_CATEGORY(EventCategory::Input | EventCategory::Mouse)
    };

    class POSITRON_API MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        explicit MouseButtonReleasedEvent(const MouseButton button) : MouseButtonEvent(button) {}

        POSITRON_EVENT_TYPE("MouseButtonReleased")
        POSITRON_EVENT_CATEGORY(EventCategory::Input | EventCategory::Mouse)
    };

    class POSITRON_API MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(const double x, const double y) : x_(x), y_(y) {}

        [[nodiscard]] double getX() const { return x_; }
        [[nodiscard]] double getY() const { return y_; }

        POSITRON_EVENT_TYPE("MouseMoved")
        POSITRON_EVENT_CATEGORY(EventCategory::Input | EventCategory::Mouse)

    private:
        double x_, y_;
    };

    class POSITRON_API MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(const double xOffset, const double yOffset) : xOffset_(xOffset), yOffset_(yOffset) {}

        [[nodiscard]] double getXOffset() const { return xOffset_; }
        [[nodiscard]] double getYOffset() const { return yOffset_; }

        POSITRON_EVENT_TYPE("MouseScrolled")
        POSITRON_EVENT_CATEGORY(EventCategory::Input | EventCategory::Mouse)

    private:
        double xOffset_, yOffset_;
    };
} // namespace Positron
