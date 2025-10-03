#pragma once

UENUM()
enum class EGameState : uint8
{
    PreGame,
    DuringGame,
    PreBoss,
    PostBoss,
    SecretRoom,
    ArenaMode
};