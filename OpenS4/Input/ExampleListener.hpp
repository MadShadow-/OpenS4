#pragma once
#include "InputListener.hpp"
#include "Events/KeyEvent.hpp"

namespace OpenS4::Input
{
    /// @brief Example on how to use the InputListener
    /// 1. Subclass it.
    /// 2. Overwrite each callback of your interest, see InputListener
    /// 3. Define Keybinds in the constructor.
    class ExampleListener : public InputListener
    {
       public:
        /// @param name A name to identify your listener.
        ExampleListener(std::string name);
        EventDispatching keyPressed(const KeyEvent* event) override;
        EventDispatching keyReleased(const KeyEvent* event) override;
        EventDispatching characterInput(const KeyEvent* event) override;
    };
}