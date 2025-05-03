#pragma once

UENUM()
enum class EHandsAnimState : uint8
{
	None,
	Reload,
	Shoot,
	ShootLeft,
	ShootRight,
	StretchHold,
	StretchStart,
	StretchRelease,
	GrenadeLaunch,
	RangerDeployShield,
	SecondaryGrenadeLaunch,
	OverCharge,
	QuickMelee
};