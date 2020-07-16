#include "KeyBinding.hpp"

namespace OpenS4::Input
{
    KeyBinding::KeyBinding(std::string identifier,
                           Key key,
                           Modifier modifier,
                           std::function<EventDispatching(const KeyEvent* keyEvent)> func)
        : m_identifier(identifier), m_key(key), m_modifier(modifier), m_func(func)
    {
    }

    s32 KeyBinding::getKeyHash(Key key, Modifier modifier) { return (s32)key | (s32)modifier; }
}  // namespace OpenS4::Input
