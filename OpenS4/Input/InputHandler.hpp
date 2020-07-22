#pragma once
#include <list>

#include "Events/MouseEvent.hpp"
#include "main.hpp"

namespace OpenS4::Input
{
    class InputListener;
    class KeyEvent;

    /// @brief Receives input events and populates them to registered InputListener objects.
    /// In a subclass dedicated to the window system, input events are catched, wrapped into OpenS4 events
    /// and populated to all registered InputListener objects.
    /// 
    /// A listener is always registered to the top. The topmost listener can decide to catch an event or to pass it.
    class InputHandler
    {
       public:
        void registerListenerAtTop(std::shared_ptr<InputListener> listener);
        void unregisterListener(std::shared_ptr<InputListener> listener);

        void populateKeyEvent(const OpenS4::Input::KeyEvent* keyEvent);
        void populateMouseEvent(const MouseEvent* keyEvent);

       protected:
        InputHandler() = default;
        InputHandler(const InputHandler&) = delete;

        static inline InputHandler* instance{nullptr};

       public:
        static InputHandler* getInstance() { return instance; }

       private:
        std::list<std::shared_ptr<InputListener>> m_listenerList;
    };

}  // namespace OpenS4::Input
