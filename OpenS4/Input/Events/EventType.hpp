#pragma once
namespace OpenS4::Input
{
    // key and mouse types
    enum class EventType
    {
        None = 0,
        Pressed,
        PressedRepeat,
        Released,
        CharacterInput,
        Moved,
        Scrolled,
    };
}