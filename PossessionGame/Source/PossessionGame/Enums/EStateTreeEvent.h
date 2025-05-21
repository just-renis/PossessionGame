#pragma once

UENUM()
enum class EStateTreeEvent : uint8
{
    None,
    MinionBossStomp,
    MinionBossLaserEye,
    RangerLaunchGrenade,
    RangerDeployShield
};