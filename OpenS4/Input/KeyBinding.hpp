#pragma once
#include <functional>

#include "../main.hpp"
#include "Events/KeyEvent.hpp"
#include "InputListener.hpp"
#include "Mapping.hpp"
#include "Modifier.hpp"

namespace OpenS4::Input
{
    class KeyBinding
    {
       public:
        KeyBinding(std::string identifier,
                   Key key,
                   Modifier modifier,
                   std::function<EventDispatching(const KeyEvent* keyEvent)> func);

        EventDispatching action(const KeyEvent* keyEvent) { return m_func(keyEvent); }

        Key getKey() { return m_key; }
        Modifier getModifier() { return m_modifier; }
        std::string& getIdentifier() { return m_identifier; }

        static s32 getKeyHash(Key key, Modifier modifier);

       private:
        std::string m_identifier;
        Key m_key;
        Modifier m_modifier;
        std::function<EventDispatching(const KeyEvent* keyEvent)> m_func;
    };
}  // namespace OpenS4::Input
