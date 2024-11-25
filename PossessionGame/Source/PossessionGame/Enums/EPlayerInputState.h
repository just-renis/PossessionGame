#pragma once

UENUM()
enum class EPlayerInputState : uint8
{
    None,
    LeftClickRelease,
    LeftClickStart,
    RightClickStart,
    RightClickRelease
};