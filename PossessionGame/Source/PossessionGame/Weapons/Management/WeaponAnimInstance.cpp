#include "WeaponAnimInstance.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

bool UWeaponAnimInstance::IsPlaying(const EWeaponAnimState InState)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        return Montage_IsPlaying(Montages.Find(InState)->Get());
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in IsPlaying"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(InState));
    return false;
}

bool UWeaponAnimInstance::IsMontageExisting(const EWeaponAnimState InState) const { return AssetsManager->GetWeaponData(WeaponName)->WeaponMontages.Contains(InState); }

float UWeaponAnimInstance::GetMontageLength(const EWeaponAnimState InState)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        switch (InState)
        {
            case EWeaponAnimState::Reload: return Montages.Find(EWeaponAnimState::Reload)->Get()->GetPlayLength();
            case EWeaponAnimState::Shoot: return Montages.Find(EWeaponAnimState::Shoot)->Get()->GetPlayLength();
            default: return Montages.Find(EWeaponAnimState::ChargeUp)->Get()->GetPlayLength();
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in GetMontageLength"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(InState));
    return 0.f;
}

void UWeaponAnimInstance::PlayMontage(const EWeaponAnimState InState, const float InStartPosition)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        Montage_Play(Montages.Find(InState)->Get(), GlobalPlayRate, EMontagePlayReturnType::MontageLength, InStartPosition);
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in PlayMontage"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(InState));
}

void UWeaponAnimInstance::StopMontage(const EWeaponAnimState InState, const float InBlendOut)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        Montage_Stop(InBlendOut, Montages.Find(InState)->Get());
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in StopMontage"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(InState));
}

void UWeaponAnimInstance::SetGlobalPlayRate(const float InPlayRate)
{
    GlobalPlayRate = InPlayRate;
    if (const TObjectPtr<UAnimMontage> CurrentMontage = GetCurrentActiveMontage()) Montage_SetPlayRate(CurrentMontage, GlobalPlayRate);
}

bool UWeaponAnimInstance::IsMontage(const EWeaponAnimState InState, const UAnimMontage* InMontage) const
{
    if (const TObjectPtr<UAnimMontage>* Found = Montages.Find(InState)) return *Found == InMontage;
    return false;
}

void UWeaponAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    AssetsManager = GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
    OnMontageBlendingOut.AddDynamic(this, &UWeaponAnimInstance::OnMontageEnd);
}

void UWeaponAnimInstance::LoadMontageIfNotLoaded(const EWeaponAnimState InState)
{
    if (!Montages.Contains(InState)) Montages.Add(InState, AssetsManager->GetWeaponData(WeaponName)->WeaponMontages[InState].LoadSynchronous());
}

void UWeaponAnimInstance::NativeUninitializeAnimation()
{
    OnMontageBlendingOut.RemoveAll(this);
    Super::NativeUninitializeAnimation();
}
