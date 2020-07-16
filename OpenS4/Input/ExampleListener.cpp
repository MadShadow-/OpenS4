#include "ExampleListener.hpp"
#include "KeyBinding.hpp"

namespace OpenS4::Input
{
    ExampleListener::ExampleListener(std::string name) : InputListener(name)
    {
        // all key bind registration goes here
        auto lambda = [](const KeyEvent* keyEvent) -> EventDispatching {
            // do smth here
            return EventDispatching::CatchEvent;
        };
        auto myKeyBind = std::make_shared<KeyBinding>(std::string("ExampleKeyBinding"), Key::X, Modifier::Control + Modifier::Shift, lambda);
        addKeyBinding(myKeyBind);
    }

    EventDispatching ExampleListener::keyPressed(const KeyEvent* event)
    {
        EventDispatching dispatched = keyBindAction(event);
        if (dispatched != EventDispatching::NoKeyBindFound)
        {
            return dispatched;
        }
        // do smth else with the event?
        return EventDispatching::PassEvent;
    }

    EventDispatching ExampleListener::keyReleased(const KeyEvent* event) { return EventDispatching::PassEvent; }

    EventDispatching ExampleListener::characterInput(const KeyEvent* event) { return EventDispatching::PassEvent; }

}  // namespace OpenS4::Input
