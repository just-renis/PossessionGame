#pragma once

UENUM()
enum class EUpgradeAttribute : uint8
{
    None,
    FlatHealth,
    FlatDamage,
    DamagePercentage,
    ProjectilesAmount,
    AbilityCooldownReduction,
    Size,
    AbilityDuration,
    EffectDuration,
    AbilityChargesAmount,
    ReduceDamageTaken,
	CloneAmount,
	Cap,
	ProjectileSpeed,
	Impulse,
	TimeDilation,
    Special1,
    Special2,
    Special3
};