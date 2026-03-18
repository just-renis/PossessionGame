#pragma once

UENUM()
enum class EDamageReason : uint8
{
    None,
    RangerDamage,
    TricksterDamage,
    SpyDamage,
    GuardianDamage,
    MinionBossDamage,
    MinionDamage,
    TricksterPoisonDamage,
    RangerGrenadeDamage,
    BreacherBashDamage,
    WraithDamage,
    WraithMolotovDamage,
    WraithEnemyMeteor,
    WraithPlayerMeteor,
    TankSlam
};