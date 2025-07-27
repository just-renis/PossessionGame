#pragma once

UENUM()
enum class EHandsAnimState : uint8
{
	None,
	Reload,
	Shoot,
	StretchHold,
	StretchStart,
	StretchRelease,
	GrenadeLaunch,
	RangerDeployShield,
	SecondaryGrenadeLaunch,
	OverCharge,
	QuickMelee,
	Jump,
	Slide
};