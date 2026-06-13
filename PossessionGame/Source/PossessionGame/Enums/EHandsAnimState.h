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
	QuickMeleeSecondCombo,
	Jump,
	Slide,
	DeployClone,
	Throw,
	BreacherBash,
	StunEffect,
	TeleportTravel,
	TeleportImpact,
	JumpSecondary,
	SlideSecondary,
	Slam,
	DashSlash,
	Deflect1,
	Deflect2,
	Deflect3,
	Deflect4,
	ShurikenThrow
};