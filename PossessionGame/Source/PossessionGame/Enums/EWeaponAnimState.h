#pragma once

UENUM()
enum class EWeaponAnimState : uint8
{
	None,
	Reload,
	Shoot,
	AIShoot,
	AIReload,
	ChargeUp,
	StretchStart,
	StretchRelease,
	StretchHold
};