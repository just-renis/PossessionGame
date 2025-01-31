#pragma once

UENUM()
enum class ESound : uint8
{
	None,
	WaveStart,
	WaveCountdown,
	RangerShoot,
	RangerReload,
	OnPossession,
	TakeDamageLayer1,
	TakeDamageLayer2,
	CriticalDamageBeep,
	CriticalHealthEnter,
	PlayerDeath,
	RangerOnPossession,
	RangerSpawn,
	RangerDeath,
	RangerDeployShield,
	RangerDissolveShield,
	RangerGrenadeLaunch,
	RangerGrenadeExplosion,
	RangerSlowMotion,
	RangerGrenadeLaunchAim,
	MinionShoot,
	MinionSpawn,
	MinionDeath,
	TricksterShoot,
	TricksterReload,
	TricksterSpawn,
	TricksterDeath,
	TricksterOnPossession
};