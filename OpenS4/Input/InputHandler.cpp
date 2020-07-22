#include "InputHandler.hpp"

#include <functional>

#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "InputListener.hpp"

namespace OpenS4::Input
{
    void InputHandler::registerListenerAtTop(std::shared_ptr<InputListener> listener)
    {
        m_listenerList.push_front(listener);
    }

    void InputHandler::unregisterListener(std::shared_ptr<InputListener> listener) { m_listenerList.remove(listener); }

    void InputHandler::populateKeyEvent(const KeyEvent* keyEvent)
    {
        switch (keyEvent->getEventType())
        {
            case EventType::Pressed:
            case EventType::PressedRepeat:
                for (const auto& listener : m_listenerList)
                {
                    if (listener->keyPressed(keyEvent) == EventDispatching::CatchEvent)
                    {
                        break;
                    }
                }
                break;

            case EventType::Released:
                for (const auto& listener : m_listenerList)
                {
                    if (listener->keyReleased(keyEvent) == EventDispatching::CatchEvent)
                    {
                        break;
                    }
                }
                break;

            case EventType::CharacterInput:
                for (const auto& listener : m_listenerList)
                {
                    if (listener->characterInput(keyEvent) == EventDispatching::CatchEvent)
                    {
                        break;
                    }
                }
                break;
            default:
                OpenS4::getLogger().err("Unhandled keyevent!");
                break;
        }
    }

    void InputHandler::populateMouseEvent(const MouseEvent* mouseEvent)
    {
        switch (mouseEvent->getEventType())
        {
            case EventType::Pressed:
            case EventType::PressedRepeat:
                for (const auto& listener : m_listenerList)
                {
                    if (listener->mouseButtonPressed(mouseEvent) == EventDispatching::CatchEvent)
                    {
                        break;
                    }
                }
                break;

            case EventType::Released:
                for (const auto& listener : m_listenerList)
                {
                    if (listener->mouseButtonReleased(mouseEvent) == EventDispatching::CatchEvent)
                    {
                        break;
                    }
                }
                break;

            case EventType::Moved:
                for (const auto& listener : m_listenerList)
                {
                    if (listener->mouseMoved(mouseEvent) == EventDispatching::CatchEvent)
                    {
                        break;
                    }
                }
                break;

            case EventType::Scrolled:
                for (const auto& listener : m_listenerList)
                {
                    if (listener->mouseScrolled(mouseEvent) == EventDispatching::CatchEvent)
                    {
                        break;
                    }
                }
                break;
            default:
                OpenS4::getLogger().err("Unhandled mouseevent!");
                break;
        }
    }

}  // namespace OpenS4::Input
