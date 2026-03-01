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
	BreacherBash,
	StunEffect,
	TeleportTravel,
	TeleportImpact,
	JumpSecondary,
	SlideSecondary
};