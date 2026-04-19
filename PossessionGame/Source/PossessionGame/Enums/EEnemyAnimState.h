#pragma once

UENUM()
enum class EEnemyAnimState : uint8
{
	None,
	Reload,
	Shoot,
	GrenadeLaunch,
	DeployShield,
	EmpEffect,
	EmpRecover,
	Stomp,
	ShootLaser,
	StopLaser,
	StunEffect,
	Bash,
	MeteorShower,
	ActivatePyroBall,
	Slash,
	Kneel,
	StandUp,
	SlamFromSky,
	SlamPrepare,
	MissilesLaunch
};