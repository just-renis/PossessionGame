#pragma once

UENUM()
enum class EStateTreeEvent : uint8
{
    None,
    MinionBossStomp,
    MinionBossLaserEye,
    MinionBossMoveDirectlyToPlayer,
    RangerLaunchGrenade,
    RangerDeployShield,
    TricksterLaunchPoisonGrenade,
    WardenBeamShot,
    BreacherBash,
    WraithMeteorShower,
    PyroBossActivateFireFloor,
    PyroBossStomp,
    PyroBossBallPhase,
    MoveDirectly,
    MoveWithPathfinder,
    SoldierBossMissilesLaunch
};