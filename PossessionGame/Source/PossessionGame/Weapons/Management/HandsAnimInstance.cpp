#include "HandsAnimInstance.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "KismetAnimationLibrary.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

bool UHandsAnimInstance::IsPlaying(const EHandsAnimState InState)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        return Montage_IsPlaying(Montages.Find(InState)->Get());
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in IsPlaying"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(InState));
    return false;
}

float UHandsAnimInstance::GetMontagePosition(const EHandsAnimState InState)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        return Montage_GetPosition(Montages.Find(InState)->Get());
    }
    UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in GetMontagePosition"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(InState));
    return 0.f;
}

void UHandsAnimInstance::PlayMontage(const EHandsAnimState InState, const float InStartPosition)
{
    if (InState == EHandsAnimState::Jump && IsAnyMontagePlaying()) return;
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        Montage_Play(Montages.Find(InState)->Get(), GlobalPlayRate, EMontagePlayReturnType::MontageLength, InStartPosition);
        if (InState == EHandsAnimState::Reload)
        {
            bIsReloading = true;
            Proprietor->SetIsActionInMotion(true);
        }
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in PlayMontage"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(InState));
}

void UHandsAnimInstance::PlayMontageWithHold(const EHandsAnimState InState, const float InHoldTime)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        PlayMontage(InState);
        if (InHoldTime > 0.f) GetWorld()->GetTimerManager().SetTimer(MontageWithHoldHandle, this, &UHandsAnimInstance::MontageWithHoldEnd, InHoldTime, false);
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in PlayMontageWithHold"), *UEnum::GetValueAsString(Proprietor->GetCharacterName()), *UEnum::GetValueAsString(InState));
}

void UHandsAnimInstance::StopMontage(const EHandsAnimState InState, const float InBlendOut)
{
    if (IsMontageExisting(InState))
    {
        LoadMontageIfNotLoaded(InState);
        Montage_Stop(InBlendOut, Montages.Find(InState)->Get());
    }
    else UE_LOG(LogTemp, Warning, TEXT("%s Montage with state: %s doesn't exist in StopMontage"), *UEnum::GetValueAsString(WeaponName), *UEnum::GetValueAsString(InState));
}

void UHandsAnimInstance::SetGlobalPlayRate(const float InPlayRate)
{
    GlobalPlayRate = InPlayRate;
    if (const TObjectPtr<UAnimMontage> CurrentMontage = GetCurrentActiveMontage()) Montage_SetPlayRate(CurrentMontage, GlobalPlayRate);
}

void UHandsAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    DelegateManager = GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>();
    AssetsManager = GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
    OnMontageBlendingOut.AddDynamic(this, &UHandsAnimInstance::OnMontageEnd);
}

void UHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
    const TObjectPtr<UAnimMontage>* ReloadMontage = Montages.Find(EHandsAnimState::Reload);
    if (ReloadMontage && InMontage == ReloadMontage->Get())
    {
        bIsReloading = false;
        Proprietor->SetIsActionInMotion(false);
        (void)GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->OnHandsReloadMontageEnd.ExecuteIfBound();
        (void)GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->OnPlayerReloadMontageEnd.ExecuteIfBound();
        return;
    }
    const TObjectPtr<UAnimMontage>* ShootMontage = Montages.Find(EHandsAnimState::Shoot);
    UE_LOG(LogTemp, Error, TEXT("SHOOT ENDED 0"));
    if (ShootMontage && InMontage == ShootMontage->Get())
    {
    UE_LOG(LogTemp, Error, TEXT("SHOOT ENDED 1"));
        if (Proprietor->GetIsSliding())
        {
    UE_LOG(LogTemp, Error, TEXT("Play with hold slide"));
            PlayMontageWithHold(EHandsAnimState::Slide, -1.f);
        }
        return;
    }
    const TObjectPtr<UAnimMontage>* QuickMeleeMontage = Montages.Find(EHandsAnimState::QuickMelee);
    if (QuickMeleeMontage && InMontage == QuickMeleeMontage->Get()) Proprietor->SetIsActionInMotion(false);
}

bool UHandsAnimInstance::IsMontageExisting(const EHandsAnimState InState)
{
    return AssetsManager->GetWeaponData(WeaponName)->HandsMontages.Contains(InState);
}

void UHandsAnimInstance::LoadMontageIfNotLoaded(const EHandsAnimState InState)
{
    if (!Montages.Contains(InState)) Montages.Add(InState, AssetsManager->GetWeaponData(WeaponName)->HandsMontages[InState].LoadSynchronous());
}

void UHandsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (Proprietor) DirectionalAngle = UKismetAnimationLibrary::CalculateDirection(Proprietor->GetVelocity(), Proprietor->GetActorRotation());
}

void UHandsAnimInstance::MontageWithHoldEnd() { StopAllMontages(0.1f); }

void UHandsAnimInstance::NativeUninitializeAnimation()
{
    OnMontageBlendingOut.RemoveAll(this);
    Super::NativeUninitializeAnimation();
}
