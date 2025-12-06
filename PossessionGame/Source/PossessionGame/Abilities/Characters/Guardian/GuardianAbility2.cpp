#include "GuardianAbility2.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Characters/Guardian.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void UGuardianAbility2::P_Execute()
{
	Proprietor->SetSecondAbilityUsageState(true);
	Dilation = Data->GetSpecificStrength() + GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(Data->GetAbilityName(), EUpgradeAttribute::TimeDilation, true);
	GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::GuardianSlowMotion, true);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f / Dilation);
	Proprietor->SetCustomTimeDilation(Dilation);
	DelegateManager->OnTimeDilationChanged.Broadcast(Dilation);
	SetTimeStopProcess();
	Super::P_Execute();
}

void UGuardianAbility2::P_StopAbility()
{
	if (!bIsAbilityInDuration) return;
	if (DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::GuardianAbility2, EUpgradeAttribute::Special1)) DelegateManager->AddActionSpeedBoost.Broadcast(0.5f, 5.f);
	Proprietor->SetSecondAbilityUsageState(false);
	P_StopTimeStopProcess();
	GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->RemoveStoredSound(ESound::GuardianSlowMotion);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	Proprietor->SetCustomTimeDilation(1.f);
	DelegateManager->OnTimeDilationChanged.Broadcast(1.f);
	Super::P_StopAbility();
}

void UGuardianAbility2::P_TimeStopProcess()
{
	if (!MID_TimeStop) return;
	TimeStopPulseDistance += 0.01f;
	MID_TimeStop->SetScalarParameterValue("PulseDistance", TimeStopPulseDistance);
	if (TimeStopPulseDistance >= 1.f)
	{
		TimeStopPulseDistance = 1.f;
		GetWorld()->GetTimerManager().ClearTimer(ProcessTimeStopHandle);
		GetWorld()->GetTimerManager().SetTimer(ProcessTimeStopHandle, this, &UGuardianAbility2::P_TimeStopRegressProcess, 0.01f / Dilation, true, (DurationTime - 1.5f) / Dilation);
	}
}

void UGuardianAbility2::P_TimeStopRegressProcess()
{
	if (!MID_TimeStop) return;
	TimeStopPulseDistance -= 0.02f;
	MID_TimeStop->SetScalarParameterValue("PulseDistance", TimeStopPulseDistance);
	if (TimeStopPulseDistance <= 0.f)
	{
		TimeStopPulseDistance = 0.f;
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::GuardianTimeStopPulse, MID_TimeStop, 0.f);
	}
}

void UGuardianAbility2::P_StopTimeStopProcess()
{
	if (!MID_TimeStop) return;
	TimeStopPulseDistance = 0.f;
	MID_TimeStop->SetScalarParameterValue("PulseDistance", 0.f);
	GetWorld()->GetTimerManager().ClearTimer(ProcessTimeStopHandle);
	GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::GuardianTimeStopPulse, MID_TimeStop, 0.f);
}

bool UGuardianAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UGuardianAbility2::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AGuardian>(AbilityProprietor); }

void UGuardianAbility2::SetTimeStopProcess()
{
	if (!MID_TimeStop) MID_TimeStop = UMaterialInstanceDynamic::Create(Proprietor->GetTimeStopMaterial(), this);
	GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::GuardianTimeStopPulse, MID_TimeStop, 1.f);
	GetWorld()->GetTimerManager().SetTimer(ProcessTimeStopHandle, this, &UGuardianAbility2::P_TimeStopProcess, 0.01f / Dilation, true);
}
