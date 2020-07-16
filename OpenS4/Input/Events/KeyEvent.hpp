#pragma once
#include "../Mapping.hpp"
#include "../Modifier.hpp"
#include "Event.hpp"
#include "EventType.hpp"
#include "Input/InputHandler.hpp"

namespace OpenS4::Input
{
    class KeyEvent : public Event
    {
       public:
        KeyEvent::KeyEvent(EventType eventType, Key key, Modifier modifier, u32 character)
            : Event(eventType), m_key(key), m_modifier(modifier), m_character(character)
        {
        }
        Key getKey() const { return m_key; }
        Modifier getModifier() const { return m_modifier; }
        u32 getChar() const { return m_character; }

       private:
        Key m_key;
        Modifier m_modifier;
        u32 m_character;
    };
}  // namespace OpenS4::Input
