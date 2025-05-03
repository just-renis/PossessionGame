#include "HandsAnimInstance.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "KismetAnimationLibrary.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

bool UHandsAnimInstance::IsPlaying(EHandsAnimState State)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        return Montage_IsPlaying(Montages.Find(State)->Get());
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in IsPlaying"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(State));
    return false;
}

float UHandsAnimInstance::GetMontagePosition(EHandsAnimState State)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        return Montage_GetPosition(Montages.Find(State)->Get());
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in GetMontagePosition"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(State));
    return 0.f;
}

void UHandsAnimInstance::PlayMontage(EHandsAnimState State, float StartPosition)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        Montage_Play(Montages.Find(State)->Get(), 1, EMontagePlayReturnType::MontageLength, StartPosition);
        if (State == EHandsAnimState::Reload) bIsReloading = true;
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in PlayMontage"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(State));
}

void UHandsAnimInstance::StopMontage(EHandsAnimState State, float BlendOut)
{
    if (IsMontageExisting(State))
    {
        LoadMontageIfNotLoaded(State);
        Montage_Stop(BlendOut, Montages.Find(State)->Get());
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in StopMontage"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(State));
}

void UHandsAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    DelegateManager = GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>();
    AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
    OnMontageBlendingOut.AddDynamic(this, &UHandsAnimInstance::OnMontageEnd);
}

void UHandsAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
    const TObjectPtr<UAnimMontage>* ReloadMontage = Montages.Find(EHandsAnimState::Reload);
    if (ReloadMontage && Montage == ReloadMontage->Get())
    {
        bIsReloading = false;
        (void)DelegateManager->OnHandsReloadMontageEnd.ExecuteIfBound();
        return;
    }
    const TObjectPtr<UAnimMontage>* QuickMeleeMontage = Montages.Find(EHandsAnimState::QuickMelee);
    if (QuickMeleeMontage && Montage == QuickMeleeMontage->Get())  Proprietor->SetActionInMotion(false);
}

bool UHandsAnimInstance::IsMontageExisting(EHandsAnimState State)
{
    return AssetsManager->GetWeaponData(WeaponName)->HandsMontages.Contains(State);
}

void UHandsAnimInstance::LoadMontageIfNotLoaded(EHandsAnimState State)
{
    if (!Montages.Contains(State)) Montages.Add(State, AssetsManager->GetWeaponData(WeaponName)->HandsMontages[State].LoadSynchronous());
}

void UHandsAnimInstance::BeginDestroy()
{
    Super::BeginDestroy();
    OnMontageBlendingOut.RemoveAll(this);
}

void UHandsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (Proprietor) DirectionalAngle = UKismetAnimationLibrary::CalculateDirection(Proprietor->GetVelocity(), Proprietor->GetActorRotation());
}
