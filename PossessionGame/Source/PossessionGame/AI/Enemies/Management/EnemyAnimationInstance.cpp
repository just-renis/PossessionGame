#include "EnemyAnimationInstance.h"
#include "Enemy.h"
#include "EnemyAIController.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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

void UEnemyAnimationInstance::PlayMontageWithHold(const EEnemyAnimState State, const float InHoldTime)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        InnerPlayMontage(State);
        UKismetSystemLibrary::K2_SetTimer(this, "MontageHoldEnd", GetMontageLength(State) + InHoldTime, false);
        FTimerHandle PauseHandle;
        TObjectPtr<UAnimMontage> Montage = *Montages.Find(State);
        GetWorld()->GetTimerManager().SetTimer(PauseHandle, [this, Montage] { Montage_Pause(Montage); }, GetMontageLength(State) - Montages.Find(State)->Get()->GetDefaultBlendOutTime() - 0.05f, false);
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

void UEnemyAnimationInstance::ClearTimers() { UKismetSystemLibrary::K2_ClearTimer(this, "MontageHoldEnd"); }

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

void UEnemyAnimationInstance::InnerPlayMontage(EEnemyAnimState State) { Montage_Play(Montages.Find(State)->Get()); }

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

void UEnemyAnimationInstance::MontageHoldEnd() const
{
    Proprietor->SetActionInMotion(false);
    if (Proprietor->GetAI()) Proprietor->GetAI()->ResumeLogic();
}
