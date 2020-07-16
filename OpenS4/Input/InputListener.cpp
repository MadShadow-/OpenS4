#include "InputListener.hpp"

#include "KeyBinding.hpp"

namespace OpenS4::Input
{
    EventDispatching InputListener::keyPressed(const KeyEvent* event) { return EventDispatching::PassEvent; }

    EventDispatching InputListener::keyReleased(const KeyEvent* event) { return EventDispatching::PassEvent; }

    EventDispatching InputListener::characterInput(const KeyEvent* event) { return EventDispatching::PassEvent; }

    EventDispatching InputListener::mouseButtonPressed(const MouseEvent* event) { return EventDispatching::PassEvent; }

    EventDispatching InputListener::mouseButtonReleased(const MouseEvent* event) { return EventDispatching::PassEvent; }

    EventDispatching InputListener::mouseMoved(const MouseEvent* event) { return EventDispatching::PassEvent; }

    EventDispatching InputListener::mouseScrolled(const MouseEvent* event) { return EventDispatching::PassEvent; }

    EventDispatching InputListener::mouseEntered(const MouseEvent* event) { return EventDispatching::PassEvent; }

    EventDispatching InputListener::keyBindAction(const KeyEvent* keyEvent)
    {
        s32 hash = KeyBinding::getKeyHash(keyEvent->getKey(), keyEvent->getModifier());
        auto keyBinding = m_keyBindings.find(hash);
        if (keyBinding != m_keyBindings.end())
        {
            return keyBinding->second->action(keyEvent);
        }
        return EventDispatching::NoKeyBindFound;
    }

    void InputListener::addKeyBinding(std::shared_ptr<OpenS4::Input::KeyBinding> keyBinding)
    {
        s32 hash = KeyBinding::getKeyHash(keyBinding->getKey(), keyBinding->getModifier());
        m_keyBindings[hash] = keyBinding;
    }

    void InputListener::removeKeyBinding(std::shared_ptr<OpenS4::Input::KeyBinding> keyBinding)
    {
        s32 hash = KeyBinding::getKeyHash(keyBinding->getKey(), keyBinding->getModifier());
        m_keyBindings.erase(hash);
    }

}  // namespace OpenS4::Input
