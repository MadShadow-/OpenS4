#pragma once
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "InputHandler.hpp"

#include <unordered_map>

namespace OpenS4::Input
{
    class KeyBinding;
    // currently not used; (allow a listener to decide for which callbacks it wants to register)
    enum class Callbacks
    {
        KeyPressed = 0x1,
        KeyReleased = 0x2,
        MouseButtonPressed = 0x4,
        MouseButtonReleased = 0x8,
        MouseMoved = 0xF,
        MouseScrolled = 0x10,
        MouseEntered = 0x20,
        CharacterInput = 0x40,
    };

    inline Callbacks operator|(Callbacks lhs, Callbacks rhs)
    {
        using T = std::underlying_type_t<Callbacks>;
        return static_cast<Callbacks>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    inline Callbacks& operator|=(Callbacks& lhs, Callbacks rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    inline Callbacks operator&(Callbacks lhs, Callbacks rhs)
    {
        using T = std::underlying_type_t<Callbacks>;
        return static_cast<Callbacks>(static_cast<T>(lhs) & static_cast<T>(rhs));
    }

    inline Callbacks& operator&=(Callbacks& lhs, Callbacks rhs)
    {
        lhs = lhs & rhs;
        return lhs;
    }

    enum class EventDispatching
    {
        // following listeners will receive event
        PassEvent = false,

        // none of the following listener retrieve the event
        CatchEvent = true,

        // no key bind for event found
        NoKeyBindFound,
    };

    /// @brief Allows listening to key/mouse input.
    /// Subclass from this class and overwrite each desired input callback.
    class InputListener
    {
       public:
        InputListener(std::string name) : m_name(name) {}

        virtual EventDispatching keyPressed(const KeyEvent* event);
        virtual EventDispatching keyReleased(const KeyEvent* event);
        virtual EventDispatching characterInput(const KeyEvent* event);
        virtual EventDispatching mouseButtonPressed(const MouseEvent* event);
        virtual EventDispatching mouseButtonReleased(const MouseEvent* event);
        virtual EventDispatching mouseMoved(const MouseEvent* event);
        virtual EventDispatching mouseScrolled(const MouseEvent* event);
        virtual EventDispatching mouseEntered(const MouseEvent* event);

        virtual ~InputListener() = default;

        /// @brief calls a Key + Modifier related action, if existent
        EventDispatching keyBindAction(const KeyEvent* keyEvent);

        void addKeyBinding(std::shared_ptr<OpenS4::Input::KeyBinding> keyBinding);
        void removeKeyBinding(std::shared_ptr<OpenS4::Input::KeyBinding> keyBinding);

       protected:
        std::string m_name;

        std::unordered_map<s32, std::shared_ptr<OpenS4::Input::KeyBinding>> m_keyBindings;

        // InputHandler class reads registered Callbacks
        friend InputHandler;
        Callbacks m_registeredCallbacks{};

        std::shared_ptr<InputListener> m_selfPtr;
    };
}  // namespace OpenS4::Input
