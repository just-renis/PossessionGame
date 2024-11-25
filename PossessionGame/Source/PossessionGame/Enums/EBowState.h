#pragma once

UENUM()
enum class EBowState : uint8
{
    None,
    StretchStart,
    StretchHold,
    StretchRelease,
    Shoot,
    ShootRelease
};