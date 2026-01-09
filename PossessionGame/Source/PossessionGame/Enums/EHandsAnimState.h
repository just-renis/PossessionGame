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
	DeployShield,
	SecondaryGrenadeLaunch,
	OverCharge,
	QuickMelee,
	Jump,
	Slide,
	DeployClone,
	Throw,
	BreacherThrow,
	BreacherBash,
	StunEffect
};