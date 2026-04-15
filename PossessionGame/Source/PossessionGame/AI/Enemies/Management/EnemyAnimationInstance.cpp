#include "EnemyAnimationInstance.h"
#include "Enemy.h"
#include "EnemyAIController.h"
#include "KismetAnimationLibrary.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_CharacterContainer.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

bool UEnemyAnimationInstance::IsPlaying(const EEnemyAnimState InState)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        return Montage_IsPlaying(Montages.Find(InState)->Get());
    }
    //UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in IsPlaying"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(InState));
    return false;
}

bool UEnemyAnimationInstance::IsMontageExisting(const EEnemyAnimState InState) const
{
    return AssetsManager->GetCharacterData(Proprietor->GetCharacterName()).GetCharacterContainer()->Montages.Contains(InState);
}

float UEnemyAnimationInstance::GetMontageLength(const EEnemyAnimState InState) 
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        return Montages.Find(InState)->Get()->GetPlayLength();
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in GetMontageLength"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(InState));
    return 0.f;
}

void UEnemyAnimationInstance::SetDirectionalAngle()
{
    DirectionalAngle = UKismetAnimationLibrary::CalculateDirection(Proprietor->GetVelocity(), Proprietor->GetActorRotation());
}

void UEnemyAnimationInstance::PlayMontage(const EEnemyAnimState InState)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        InnerPlayMontage(InState);
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in PlayMontage"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(InState));
}

void UEnemyAnimationInstance::PlayMontageWithHold(const EEnemyAnimState InState, const float InHoldTime, const bool bNeedsToStopAILogic)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        InnerPlayMontage(InState);
        if (Proprietor) Proprietor->SetIsActionInMotion(true);
        MontageStateInHold = InState;
        if (bNeedsToStopAILogic && Proprietor && Proprietor->GetAI()) Proprietor->GetAI()->StopLogic(false);
        UE_LOG(LogTemp, Error, TEXT("IN HOLD TIME: %f"), InHoldTime);
        GetWorld()->GetTimerManager().SetTimer(MontageWithHoldHandle, this, &UEnemyAnimationInstance::MontageWithHoldEnd, InHoldTime);
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in PlayMontageWithHold"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(InState));
}

void UEnemyAnimationInstance::StopMontage(const EEnemyAnimState InState, const float InBlendOut)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        Montage_Stop(InBlendOut, Montages.Find(InState)->Get());
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in StopMontage"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(InState));
}

void UEnemyAnimationInstance::ClearTimers() { GetWorld()->GetTimerManager().ClearTimer(MontageWithHoldHandle); }

void UEnemyAnimationInstance::ClearMontageWithHoldHandle() { GetWorld()->GetTimerManager().ClearTimer(MontageWithHoldHandle); }

void UEnemyAnimationInstance::SetGlobalPlayRate(const float InRate)
{
    GlobalPlayRate = InRate;
    SetCurrentPlayRate();
}

bool UEnemyAnimationInstance::IsMontage(const EEnemyAnimState InState, const UAnimMontage* InMontage) const
{
    if (const TObjectPtr<UAnimMontage>* Found = Montages.Find(InState)) return *Found == InMontage;
    return false;
}

void UEnemyAnimationInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    AssetsManager = GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
    SetProprietor();
    AssignOnMontageEnd();
}

void UEnemyAnimationInstance::NativeUninitializeAnimation()
{
    OnMontageBlendingOut.RemoveAll(this);
    Super::NativeUninitializeAnimation();
}

void UEnemyAnimationInstance::AssignOnMontageEnd() { OnMontageBlendingOut.AddDynamic(this, &UEnemyAnimationInstance::OnMontageEnd); }

void UEnemyAnimationInstance::SetProprietor() { Proprietor = Cast<AEnemy>(GetOwningActor()); }

void UEnemyAnimationInstance::InnerPlayMontage(const EEnemyAnimState InState) { Montage_Play(Montages.Find(InState)->Get(), GlobalPlayRate); }

void UEnemyAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted)
{
    if (IsMontage(EEnemyAnimState::Shoot, InMontage))
    {
        Proprietor->AI_SetCanShoot(false);
        return;
    }
    if (!IsMontage(EEnemyAnimState::StunEffect, InMontage)) return;
    SetNonBlendableSlotState(false);
	SetIsControlRigEnabled(true);
}

void UEnemyAnimationInstance::NativeUpdateAnimation(const float InDeltaSeconds)
{
    Super::NativeUpdateAnimation(InDeltaSeconds);
    if (Proprietor) SetDirectionalAngle();
}

void UEnemyAnimationInstance::LoadMontageIfNotLoaded(const EEnemyAnimState InState)
{
    if (!Montages.Contains(InState)) Montages.Add(InState, AssetsManager->GetCharacterData(Proprietor->GetCharacterName()).GetCharacterContainer()->Montages[InState].LoadSynchronous());
}

void UEnemyAnimationInstance::SetCurrentPlayRate()
{
    if (const TObjectPtr<UAnimMontage> CurrentMontage = GetCurrentActiveMontage()) Montage_SetPlayRate(CurrentMontage, GlobalPlayRate);
}
