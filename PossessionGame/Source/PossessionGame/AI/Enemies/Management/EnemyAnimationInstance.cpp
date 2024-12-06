#include "EnemyAnimationInstance.h"
#include "Enemy.h"
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
        switch (State)
        {
            case EEnemyAnimState::Reload: return Montages.Find(EEnemyAnimState::Reload)->Get()->GetPlayLength();
            case EEnemyAnimState::Shoot: return Montages.Find(EEnemyAnimState::Shoot)->Get()->GetPlayLength();
            default: return 0.f;
        }
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

void UEnemyAnimationInstance::StopMontage(EEnemyAnimState State, float BlendOut)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        Montage_Stop(BlendOut, Montages.Find(State)->Get());
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in StopMontage"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(State));
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

void UEnemyAnimationInstance::InnerPlayMontage(EEnemyAnimState State) { Montage_Play(Montages.Find(State)->Get()); }

void UEnemyAnimationInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
    if (Montages.Contains(EEnemyAnimState::Shoot) && Montage == Montages.Find(EEnemyAnimState::Shoot)->Get()) Proprietor->AI_SetCanShoot(false);
    else if (Montages.Contains(EEnemyAnimState::Reload) && Montage == Montages.Find(EEnemyAnimState::Reload)->Get()) Proprietor->AI_InitiateShooting();
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
