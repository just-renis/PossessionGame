#include "WeaponAnimInstance.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

bool UWeaponAnimInstance::IsPlaying(EWeaponAnimState State)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        return Montage_IsPlaying(Montages.Find(State)->Get());
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in IsPlaying"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(State));
    return false;
}

bool UWeaponAnimInstance::IsMontageExisting(EWeaponAnimState State)
{
    return AssetsManager->GetWeaponData(WeaponName)->WeaponMontages.Contains(State);
}

float UWeaponAnimInstance::GetMontageLength(EWeaponAnimState State)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        switch (State)
        {
            case EWeaponAnimState::Reload: return Montages.Find(EWeaponAnimState::Reload)->Get()->GetPlayLength();
            case EWeaponAnimState::Shoot: return Montages.Find(EWeaponAnimState::Shoot)->Get()->GetPlayLength();
            default: return Montages.Find(EWeaponAnimState::ChargeUp)->Get()->GetPlayLength();
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in GetMontageLength"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(State));
    return 0.f;
}

void UWeaponAnimInstance::PlayMontage(EWeaponAnimState State, float StartPosition)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        Montage_Play(Montages.Find(State)->Get(), GlobalPlayRate, EMontagePlayReturnType::MontageLength, StartPosition);
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in PlayMontage"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(State));
}

void UWeaponAnimInstance::StopMontage(EWeaponAnimState State, float BlendOut)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        Montage_Stop(BlendOut, Montages.Find(State)->Get());
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in StopMontage"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(State));
}

void UWeaponAnimInstance::SetGlobalPlayRate(const float InPlayRate)
{
    GlobalPlayRate = InPlayRate;
    if (const TObjectPtr<UAnimMontage> CurrentMontage = GetCurrentActiveMontage()) Montage_SetPlayRate(CurrentMontage, GlobalPlayRate);
}

void UWeaponAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
    const TObjectPtr<UAnimMontage>* ShootMontage = Montages.Find(EWeaponAnimState::Shoot);
    if (ShootMontage && Montage == ShootMontage->Get())
    {
        if (const TObjectPtr<USkeletalMeshComponent> Mesh = GetSkelMeshComponent()) Mesh->SetComponentTickEnabled(true);
    }
}

void UWeaponAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    AssetsManager = GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
    OnMontageBlendingOut.AddDynamic(this, &UWeaponAnimInstance::OnMontageEnd);
}

void UWeaponAnimInstance::LoadMontageIfNotLoaded(EWeaponAnimState State)
{
    if (!Montages.Contains(State)) Montages.Add(State, AssetsManager->GetWeaponData(WeaponName)->WeaponMontages[State].LoadSynchronous());
}

void UWeaponAnimInstance::NativeUninitializeAnimation()
{
    OnMontageBlendingOut.RemoveAll(this);
    Super::NativeUninitializeAnimation();
}
