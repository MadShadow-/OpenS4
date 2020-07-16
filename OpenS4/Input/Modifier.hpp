#pragma once
namespace OpenS4::Input
{
    enum class Modifier
    {
        None = 0,
        ShiftLeft = 0x10000000,
        ShiftRight = 0x20000000,
        Shift = (ShiftLeft | ShiftRight),

        AltLeft = 0x40000000,
        AltRight = 0x01000000,
        Alt = (AltLeft | AltRight),

        ControlLeft = 0x02000000,
        ControlRight = 0x04000000,
        Control = (ControlLeft | ControlRight),

        CapsLock = 0x08000000,
        NumPad = 0x00100000,
    };

    inline Modifier operator|(Modifier lhs, Modifier rhs)
    {
        using T = std::underlying_type_t<Modifier>;
        return static_cast<Modifier>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    inline Modifier& operator|=(Modifier& lhs, Modifier rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    inline Modifier operator&(Modifier lhs, Modifier rhs)
    {
        using T = std::underlying_type_t<Modifier>;
        return static_cast<Modifier>(static_cast<T>(lhs) & static_cast<T>(rhs));
    }

    inline Modifier& operator&=(Modifier& lhs, Modifier rhs)
    {
        lhs = lhs & rhs;
        return lhs;
    }

    inline Modifier operator+(Modifier lhs, Modifier rhs) {
        using T = std::underlying_type_t<Modifier>;
        return static_cast<Modifier>(static_cast<T>(lhs) + static_cast<T>(rhs));
    }

}  // namespace OpenS4::Input
