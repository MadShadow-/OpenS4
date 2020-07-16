#pragma once
#include "../Mapping.hpp"
#include "Event.hpp"
#include "Input/InputHandler.hpp"

namespace OpenS4::Input
{
    // all mouse events are in ONE event for now, so query the values that make sense
    class MouseEvent : public Event
    {
       public:
        MouseEvent(
            EventType eventType, double x, double y, MouseButton button, bool entered, double scrollOffset, Modifier modifier)
            : Event(eventType),
              m_x(x),
              m_y(y),
              m_button(button),
              m_entered(entered),
              m_scrollOffset(scrollOffset),
              m_modifier(modifier)
        {
        }

        double getCursorX() const { return m_x; }
        double getCursorY() const { return m_y; }
        bool left() const { return m_button == MouseButton::Left; }
        bool right() const { return m_button == MouseButton::Right; }
        bool middle() const { return m_button == MouseButton::Middle; }
        bool entered() const { return m_entered == 1; }
        Modifier getModifier() const { return m_modifier; }
        double getMouseScroll() const { return m_scrollOffset; }

       private:
        double m_x{};
        double m_y{};
        MouseButton m_button{MouseButton::None};
        Modifier m_modifier{Modifier::None};
        bool m_entered{};
        double m_scrollOffset{};
    };
}  // namespace OpenS4::Input::Events
