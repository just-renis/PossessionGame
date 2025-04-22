#include "EnemyAnimationInstance.h"
#include "Enemy.h"
#include "EnemyAIController.h"
#include "KismetAnimationLibrary.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_CharacterContainer.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

bool UEnemyAnimationInstance::IsPlaying(EEnemyAnimState State)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        return Montage_IsPlaying(Montages.Find(State)->Get());
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in IsPlaying"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(State));
    return false;
}

float UEnemyAnimationInstance::GetMontageLength(EEnemyAnimState State) 
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        return Montages.Find(State)->Get()->GetPlayLength();
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in GetMontageLength"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(State));
    return 0.f;
}

void UEnemyAnimationInstance::SetDirectionalAngle()
{
    DirectionalAngle = UKismetAnimationLibrary::CalculateDirection(Proprietor->GetVelocity(), Proprietor->GetActorRotation());
}

void UEnemyAnimationInstance::PlayMontage(EEnemyAnimState State)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        InnerPlayMontage(State);
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in PlayMontage"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(State));
}

void UEnemyAnimationInstance::PlayMontageWithHold(const EEnemyAnimState State, const float InHoldTime, const bool bDisableActionInMotion)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        InnerPlayMontage(State);
        GetWorld()->GetTimerManager().SetTimer(MontageHoldEndHandle, [this, bDisableActionInMotion]
        { MontageHoldEnd(bDisableActionInMotion); }, GetMontageLength(State) + InHoldTime, false);
        TObjectPtr<UAnimMontage> Montage = *Montages.Find(State);
        GetWorld()->GetTimerManager().SetTimer(PauseHandle, [this, Montage]
        { UE_LOG(LogTemp, Warning, TEXT("PAUSE HAPPENED")); Montage_Pause(Montage); }, GetMontageLength(State) - Montages.Find(State)->Get()->GetDefaultBlendOutTime() - 0.02f, false);
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in PlayMontageWithHold"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(State));
}

void UEnemyAnimationInstance::StopMontage(EEnemyAnimState State, float BlendOut)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        Montage_Stop(BlendOut, Montages.Find(State)->Get());
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in StopMontage"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(State));
}

void UEnemyAnimationInstance::ClearTimers()
{
    GetWorld()->GetTimerManager().ClearTimer(MontageHoldEndHandle);
    GetWorld()->GetTimerManager().ClearTimer(PauseHandle);
}

bool UEnemyAnimationInstance::IsMontageExisting(EEnemyAnimState State)
{
    return AssetsManager->GetCharacterData(Proprietor->GetCharacterName())->Montages.Contains(State);
}

void UEnemyAnimationInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
    SetProprietor();
    AssignOnMontageEnd();
}

void UEnemyAnimationInstance::BeginDestroy()
{
    Super::BeginDestroy();
    OnMontageBlendingOut.RemoveAll(this);
}

void UEnemyAnimationInstance::AssignOnMontageEnd() { OnMontageBlendingOut.AddDynamic(this, &UEnemyAnimationInstance::OnMontageEnd); }

void UEnemyAnimationInstance::SetProprietor() { Proprietor = Cast<AEnemy>(GetOwningActor()); }

void UEnemyAnimationInstance::InnerPlayMontage(EEnemyAnimState State)
{
    UE_LOG(LogTemp, Warning, TEXT("PLAYING MONTAGE: %s"), *UEnum::GetValueAsString(State));
    Montage_Play(Montages.Find(State)->Get());
}

void UEnemyAnimationInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
    if (Montages.Contains(EEnemyAnimState::Shoot) && Montage == Montages.Find(EEnemyAnimState::Shoot)->Get()) Proprietor->AI_SetCanShoot(false);
}

void UEnemyAnimationInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (Proprietor) SetDirectionalAngle();
}

void UEnemyAnimationInstance::LoadMontageIfNotLoaded(EEnemyAnimState State)
{
    if (!Montages.Contains(State)) Montages.Add(State, AssetsManager->GetCharacterData(Proprietor->GetCharacterName())->Montages[State].LoadSynchronous());
}

void UEnemyAnimationInstance::MontageHoldEnd(const bool bDisableActionInMotion) const
{
    if (bDisableActionInMotion) Proprietor->SetActionInMotion(false);
    if (Proprietor->GetAI()) Proprietor->GetAI()->ResumeLogic();
}
