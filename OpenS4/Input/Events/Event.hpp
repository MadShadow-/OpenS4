#pragma once
#include "../Mapping.hpp"
#include "../Modifier.hpp"
#include "EventType.hpp"

namespace OpenS4::Input
{
    class Event
    {
       public:
        Event::Event(EventType eventType)
            : m_eventType(eventType)
        {
        }
        EventType getEventType() const { return m_eventType; }

       protected:
        EventType m_eventType{EventType::None};
    };
}  // namespace OpenS4::Input
