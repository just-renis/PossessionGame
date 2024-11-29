#include "Enemy.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyAIController.h"
#include "EnemyAnimationInstance.h"
#include "GameplayTagsManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PossessionGame/AI/Navigation/NavAreaModifier.h"
#include "PossessionGame/Data Assets/AI/Base Data/AICharacterData.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerCharacterData.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_CharacterContainer.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/AIHpBarUI.h"
#include "PossessionGame/UI/PauseMenuUI.h"
#include "PossessionGame/UI/PlayerHUD.h"
#include "PossessionGame/UI/SettingsUI.h"
#include "PossessionGame/Utilities/Global Utilities/Destructible.h"
#include "PossessionGame/Utilities/Global Utilities/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/AbilityManager.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	Camera->SetupAttachment(RootComponent);
	AI_HpBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("AI_HpBar"));
	AI_HpBarComponent->SetupAttachment(RootComponent);
}

bool AEnemy::GetIsPossessable() const { return AI_Data->GetIsPossessable(); }

float AEnemy::GetSkeletalMeshHeight() const { return GetMesh()->GetSocketLocation(TEXT("Head")).Z - GetMesh()->GetComponentLocation().Z; }

void AEnemy::AI_InitiateShooting() { if (CurrentMoveMode != EMoveMode::Run && !AnimInstance->IsPlaying(EEnemyAnimState::Reload)) AI_PlayShootMontage(); }

void AEnemy::AI_ShootLogic() { if (bCanShoot && !AnimInstance->IsPlaying(EEnemyAnimState::Reload) && CurrentMoveMode != EMoveMode::Run) AI_Shoot(); }

void AEnemy::AI_ReloadLogic() { if (Weapon->GetIsReloadEnabled() && bCanReload) AI_Reload(); }

void AEnemy::AI_UseAbility(const int32 AbilityIndex) { if (AbilityIndex < Abilities.Num()) Abilities[AbilityIndex]->AI_Execute(); }

void AEnemy::AI_SetMoveMode(const EMoveMode InMoveMode)
{
	CurrentMoveMode = InMoveMode;
	SetMaxWalkSpeed(InMoveMode == EMoveMode::Walk ? AI_Data->GetMaxWalkSpeed() : AI_Data->GetMaxRunSpeed());
}

void AEnemy::P_UseAbility(const int32 AbilityIndex)
{
	if (AbilityIndex < Abilities.Num() && Abilities[AbilityIndex]->CanExecute())
	{
		Abilities[AbilityIndex]->P_Execute();
		if (P_HUD && Abilities[AbilityIndex]->GetIsCooldownKill()) P_HUD->UpdateAbilityCooldown(AbilityIndex, Abilities[AbilityIndex]->GetCurrentCooldownKill());
	}
}

void AEnemy::P_UpdateAmmoHUD(const int32 WeaponAmmo) const { if (P_HUD) P_HUD->UpdateAmmoText(WeaponAmmo); }

void AEnemy::P_UpdateAmmoHUD(const int32 WeaponAmmo, const int32 MaxAmmo) const { if (P_HUD) P_HUD->UpdateAmmoText(WeaponAmmo, MaxAmmo); }

void AEnemy::P_UpdateAmmoHUD(const int32 FirstWeaponAmmo, const int32 SecondWeaponAmmo, const int32 MaxAmmo) const { if (P_HUD) P_HUD->UpdateAmmoText(FirstWeaponAmmo, SecondWeaponAmmo, MaxAmmo); }

void AEnemy::P_StopTime()
{
	if (FMath::IsNearlyEqual(UGameplayStatics::GetGlobalTimeDilation(GetWorld()), 1.f, 0.01f)) UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Cast<UPossessionGameInstance>(GetGameInstance())->GetTimeDilation());
	else UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}

void AEnemy::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsAlive)
	{
		if (Abilities.Num() > 0) TickAbilities(DeltaTime);
		if (P_HUD) P_HUD->UpdateFps(FMath::RoundToInt32(1.f / DeltaTime));
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (const TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(0).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Move);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(4).Action, ETriggerEvent::Triggered, this, &AEnemy::Jump);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(5).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Rotate);
		if (bCanRun)
		{
			EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Started, this, &AEnemy::P_ToggleRunning, true);
			EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(6).Action, ETriggerEvent::Completed, this, &AEnemy::P_ToggleRunning, false);
		}
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(7).Action, ETriggerEvent::Triggered, this, &AEnemy::P_PossessEnemy);
		P_SetupLeftClickInput(EnhancedInputComponent);
		if (bCanReload) EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(9).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Reload);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(10).Action, ETriggerEvent::Started, this, &AEnemy::P_ToggleGodMode);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(11).Action, ETriggerEvent::Started, this, &AEnemy::ToggleCrouch);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(12).Action, ETriggerEvent::Started, this, &AEnemy::P_UseAbility, 0);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(13).Action, ETriggerEvent::Started, this, &AEnemy::P_UseAbility, 1);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(14).Action, ETriggerEvent::Started, this, &AEnemy::P_UseAbility, 2);
		P_SetupRightClickInput(EnhancedInputComponent);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(16).Action, ETriggerEvent::Started, this, &AEnemy::P_HandleEscapeClick);
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(17).Action, ETriggerEvent::Started, this, &AEnemy::P_StopTime);
	}
}

void AEnemy::PossessedBy(AController* NewController)
{
	if (!bIsPossessed)
	{
		Super::PossessedBy(NewController);
		if (Cast<APlayerController>(NewController))
		{
			P_TriggerGlitchingEffect();
			P_ToggleCriticallyDamagedEffects(0.f);
			AssetsManager->RemoveStoredSound(ESound::CriticalDamageBeep);
			P_Setup();
		}
	}
}

void AEnemy::ToggleCrouch()
{
	if (GetCharacterMovement()->CanEverCrouch())
	{
		AnimInstance->SetIsCrouching(!AnimInstance->GetIsCrouching());
		if (AnimInstance->GetIsCrouching()) Crouch();
		else UnCrouch();
	}
}

void AEnemy::AI_SetCanShoot(const bool bState)
{
	bCanShoot = bState;
	if (!bCanShoot)
	{
		Weapon->SubtractShootCycleUntilReload();
		Weapon->GetShootCyclesUntilReload() <= 0 ? AI_ReloadLogic() : AI_SetShootPause();
	}
}

void AEnemy::InitiateDeath()
{
	ClearTimers();
	AI_DestroyHpBar();
	bIsAlive = false;
	if (!bIsPossessed)
	{
		DestroyAI();
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnEnemyDeath.Broadcast();
	}
	else P_DisableEnemy();
	HandleDeactivation();
	UKismetSystemLibrary::K2_SetTimer(this, "DestroyThis", 5.f, false);
}

void AEnemy::ReceiveDamage(const float Damage)
{
	CurrentHp -= Damage;
	if (P_HUD) P_HandlingDamageTaken();
	else AI_ReceivedDamage();
	if (bIsAlive && CurrentHp <= 0)
	{
		if (bIsPossessed) P_Death();
		InitiateDeath();
	}
}

void AEnemy::AI_SetupAbilities() { SetupAbilities(AssetsManager->GetCharacterData(CharacterName)->AI_Abilities_Data); }

void AEnemy::AI_Setup()
{
	AI_LoadDataIfNotLoaded(CharacterName);
	AI_SetController();
	AI->SetProprietor(this);
	AI_SetupWeapon();
	AI->SetupAI();
	AI_SetupData();
	if (Weapon->GetIsReloadEnabled()) AI->SetReloadDelay(AnimInstance->GetMontageLength(EEnemyAnimState::Reload));
	AI_SetupAbilities();
}

void AEnemy::AI_SetupWeapon()
{
	Weapon->SetProprietor(this);
	AI->SetShootCooldown(Weapon->GetShootCooldown());
	Weapon->PostProprietorSetup();
}

void AEnemy::AI_Shoot() { Weapon->AI_Shoot(); }

void AEnemy::AI_LoadDataIfNotLoaded(const ECharacterType PrimaryDataAssetName)
{
	if (!AI_Data) AI_Data = AssetsManager->GetCharacterData(PrimaryDataAssetName)->AI_Data.LoadSynchronous();
}

void AEnemy::AI_SetController() { AI = Cast<AEnemyAIController>(GetController()); }

void AEnemy::AI_PlayShootMontage()
{
	if (!bShootPauseActive && AnimInstance->IsMontageExisting(EEnemyAnimState::Shoot) && !AnimInstance->IsPlaying(EEnemyAnimState::Shoot))
	{
		AnimInstance->PlayMontage(EEnemyAnimState::Shoot);
		AI_SetCanShoot(true);
	}
}

void AEnemy::P_SetupAbilities()
{
	Abilities.Empty();
	SetupAbilities(AssetsManager->GetCharacterData(CharacterName)->P_Abilities_Data);
}

void AEnemy::P_Setup()
{
	GetMesh()->SetCollisionProfileName("Player");
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->SetTarget(this);
	P_LoadDataIfNotLoaded(CharacterName);
	bUseControllerRotationYaw = true;
	SetupStimuliSource();
	GetCharacterMovement()->bUseRVOAvoidance = false;
	P_SetupNavArea();
	P_SetupData();
	Camera->SetActive(true);
	bIsPossessed = true;
	bCanShoot = true;
	UKismetSystemLibrary::K2_SetTimer(this, "P_ControlLogic", 0.05f, true);
	Weapon->P_SetupTransfer();
	GetMesh()->SetHiddenInGame(true);
	P_InputManager = Cast<UEnhancedPlayerInput>(GetWorld()->GetFirstPlayerController()->PlayerInput);
	DestroyAI();
	P_SetupAbilities();
	P_SetupUI();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnEnemyDeath.Broadcast();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnEnemyDeath.AddUObject(this, &AEnemy::P_OnEnemyDeathEvents);
}

void AEnemy::P_Shoot(const bool bFirstWeapon)
{
	if (bCanShoot)
	{
		if (Weapon->P_CanShoot(bFirstWeapon)) Weapon->P_Shoot(bFirstWeapon);
		else if (Weapon->P_CanReload() && Weapon->GetCurrentAmmoAmount() <= 0) Weapon->P_Reload();
	}
}

void AEnemy::P_SetupLeftClickInput(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	P_SetupLeftClickInputStartedState(LeftClickInputComponent);
	P_SetupLeftClickInputTriggeredState(LeftClickInputComponent);
	P_SetupLeftClickInputCompletedState(LeftClickInputComponent);
}

void AEnemy::P_SetupLeftClickInputStartedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &AEnemy::P_TemporallyToggleRunning, true);
}

void AEnemy::P_SetupLeftClickInputTriggeredState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Triggered, this, &AEnemy::P_Shoot, true);
}

void AEnemy::P_SetupLeftClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Completed, this, &AEnemy::P_TemporallyToggleRunning, false);
}

void AEnemy::P_SetupRightClickInput(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	P_SetupRightClickInputStartedState(RightClickInputComponent);
	P_SetupRightClickInputTriggeredState(RightClickInputComponent);
	P_SetupRightClickInputCompletedState(RightClickInputComponent);
}

void AEnemy::P_DisableEnemy()
{
	GetMesh()->SetHiddenInGame(false);
	StimuliSource->UnregisterFromPerceptionSystem();
	Weapon->SetToOriginalSettings();
	GetMesh()->SetCollisionProfileName("Enemy");
	NavAreaModifier->Destroy();
	if (bPlayerDied) return;
	P_HUD->Reset();
	P_HUD = nullptr;
}

void AEnemy::P_LoadDataIfNotLoaded(const ECharacterType PrimaryDataAssetName)
{
	if (!P_Data) P_Data = AssetsManager->GetCharacterData(PrimaryDataAssetName)->P_Data.LoadSynchronous();
}

void AEnemy::P_TemporallyToggleRunning(const bool bState) { if (bIsRunning) SetMaxWalkSpeed(bState ? P_Data->GetMaxWalkSpeed() : P_Data->GetMaxRunSpeed()); }

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	AI_SetupHpBar();
	AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
	GetWorld()->GetSubsystem<UEnemyCommunicator>()->AddEnemy(this);
	SetMesh(CharacterName);
	SetAnimInstance();
	GameplayTags.AddTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Enemy")));
	Tags.Add("Enemy");
	Camera->SetActive(false);
	UKismetSystemLibrary::K2_SetTimer(this, "CheckIfFallOutOfWorld", 2.f, true);
	UKismetSystemLibrary::K2_SetTimer(this, "AI_SetCharacterMovementSpeed", 0.05f, true);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnXRayThroughWallsChange.AddUObject(this, &AEnemy::ChangeXRayState);
	AI_Setup();
}

void AEnemy::SetAnimInstance() { AnimInstance = Cast<UEnemyAnimationInstance>(GetMesh()->GetAnimInstance()); }

void AEnemy::SetMaxWalkSpeed(const float Speed) const
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
	GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity.GetSafeNormal() * Speed;
}

void AEnemy::SetInitialWalkSpeed(const bool bState) const { SetMaxWalkSpeed(bState ? P_Data->GetMaxWalkSpeed() : P_Data->GetMaxSlowedSpeed()); }

void AEnemy::SetupAbilities(const TMap<EAbility, TSoftObjectPtr<UAbilityData>>& AbilityData)
{
	if (AbilityData.Num() > 0)
	{
		const TObjectPtr<UAbilityManager> AbilityManager = GetWorld()->GetGameInstance()->GetSubsystem<UAbilityManager>();
		const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
		for (int32 i = 1; i <= AbilityData.Num(); i++)
		{
			if (TObjectPtr<UAbility> Ability = AbilityManager->GetAbility(GetAbilityFromCharacterName(i), this))
			{
				Ability->Setup(this, AbilityData[GetAbilityFromCharacterName(i)].LoadSynchronous(), DelegateManager);
				Abilities.Add(Ability);
			}
		}
		for (int32 i = 1; i <= Abilities.Num(); i++)
		{
			const int32 RefreshableAbilityIndex = AbilityData[GetAbilityFromCharacterName(i)]->GetRefreshableAbilityIndex();
			Abilities[i - 1]->SetupAdditionalProperties(AbilityData[GetAbilityFromCharacterName(i)].LoadSynchronous(), RefreshableAbilityIndex != -1 ? Abilities[RefreshableAbilityIndex] : nullptr);
		}
	}
}

void AEnemy::AttachDestructible(const TObjectPtr<ADestructible>& Destructible, const FName SocketName) const
{
	Destructible->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void AEnemy::HandleDeactivation()
{
	Weapon->DeactivateWeapon();
	GetCapsuleComponent()->DestroyComponent();
	GetMesh()->SetAnimInstanceClass(nullptr);
	GetMesh()->SetSimulatePhysics(true);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ReceiveDamage(Damage);
	if (GetMesh()->IsSimulatingPhysics())
	{
		if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
		{
			const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
			GetMesh()->AddRadialImpulse(RadialDamageEvent->Origin, RadialDamageEvent->Params.OuterRadius, 200000.f, RIF_Linear);
		}
		else if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
			GetMesh()->AddImpulseAtLocation(PointDamageEvent->ShotDirection * 10000.f, PointDamageEvent->HitInfo.Location);
		}
	}
	return Damage;
}

EAbility AEnemy::GetAbilityFromCharacterName(const int32 AbilityIndex) const
{
	switch (CharacterName)
	{
		case ECharacterType::Spy:
			switch (AbilityIndex)
			{
				case 1: return EAbility::SpyAbility1;
				case 2: return EAbility::SpyAbility2;
				default: return EAbility::SpyAbility3;
			}
		case ECharacterType::Ranger:
			switch (AbilityIndex)
			{
				case 1: return EAbility::RangerAbility1;
				case 2: return EAbility::RangerAbility2;
				default: return EAbility::RangerAbility3;
			}
		default: return EAbility::None; 
	}
}

void AEnemy::AI_SetupData()
{
	MaxHp = AI_Data->GetMaxHp();
	CurrentHp = MaxHp;
	SetMaxWalkSpeed(AI_Data->GetMaxWalkSpeed());
}

void AEnemy::AI_Reload() const
{
	AnimInstance->PlayMontage(EEnemyAnimState::Reload);
	if (Weapon->GetAnimInstance()) Weapon->GetAnimInstance()->PlayMontage(EWeaponAnimState::AIReload);
	Weapon->AI_ResetShootCycles();
}

void AEnemy::AI_StopAllAbilities()
{
	for (int32 i = 0; i < Abilities.Num(); i++) Abilities[i]->AI_StopAbility();
}

void AEnemy::AI_SetShootPause()
{
	bShootPauseActive = true;
	UKismetSystemLibrary::K2_SetTimer(this, "AI_ClearShootPause", AI_Data->GetShootPause(), false);
}

void AEnemy::AI_SetupHpBar()
{
	AI_HpBar = Cast<UAIHpBarUI>(AI_HpBarComponent->GetWidget());
	AI_HideHpBar();
}

void AEnemy::AI_ReceivedDamage()
{
	if (!AI_HpBar) return;
	AI_HpBar->UpdateHpBar(CurrentHp, MaxHp);
	AI_HpBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UKismetSystemLibrary::K2_SetTimer(this, "AI_HideHpBar", 2.f, false);
}

void AEnemy::AI_DestroyHpBar()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "AI_HideHpBar");
	AI_HpBar = nullptr;
	AI_HpBarComponent = nullptr;
}

void AEnemy::AI_HideHpBar()
{
	if (AI_HpBar) AI_HpBar->SetVisibility(ESlateVisibility::Collapsed);
}

void AEnemy::AI_ClearShootPause()
{
	bShootPauseActive = false;
	UKismetSystemLibrary::K2_ClearTimer(this, "AI_ClearShootPause");
	AI_InitiateShooting();
}

void AEnemy::AI_SetCharacterMovementSpeed() { AnimInstance->SetSpeed(GetCharacterMovement()->Velocity.Size()); }

void AEnemy::P_SetupData()
{
	P_Data->SetIsGodModeEnabled(GetWorld()->GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetIsGodModeEnabled());
	MaxHp = P_Data->GetMaxHp();
	CurrentHp = MaxHp;
	SetMaxWalkSpeed(P_Data->GetMaxWalkSpeed());
}

void AEnemy::P_SetupUI()
{
	P_HUD = GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD);
	P_HUD->UpdateProgressBar(EUIBar::Possession, 0.f, P_Data->GetPossessionBarCooldown());
	P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
	P_HUD->ConfigureUIs();
	P_UpdateAbilitiesUI();
	Weapon->P_UpdateAmmoTextHUD();
	P_SetupColorsForUI();
	UKismetSystemLibrary::K2_SetTimer(this, "P_DetectIfPlayerPointingAtEnemy", 0.25f, true);
}

void AEnemy::P_PossessEnemy()
{
	if (bCanPossessOtherEnemy)
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		const FVector StartLocation = Camera->GetComponentLocation();
		QueryParams.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, StartLocation + Camera->GetForwardVector() * 15000.f, ECC_GameTraceChannel5, QueryParams)) P_PossessionLogic(Cast<AEnemy>(HitResult.GetActor()));
	}
}

void AEnemy::P_PossessionLogic(const TObjectPtr<AActor>& InActor)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(InActor))
	{
		if (Enemy->AI_Data->GetIsPossessable() && !Enemy->bIsPossessed && Enemy->bIsAlive)
		{
			P_StopAllAbilities();
			GetGameInstance()->GetSubsystem<UDelegateManager>()->OnEnemyDeath.RemoveAll(this);
			Controller->Possess(Enemy);
			InitiateDeath();
		}
	}
}

void AEnemy::P_PossessionBarLogic()
{
	PossessionBarTimer += 0.05f;
	if (PossessionBarTimer >= P_Data->GetPossessionBarCooldown()) bCanPossessOtherEnemy = true;
	else if (P_HUD) P_HUD->UpdateProgressBar(EUIBar::Possession, PossessionBarTimer, P_Data->GetPossessionBarCooldown());
}

void AEnemy::P_Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AEnemy::P_Rotate(const FInputActionValue& Value)
{
	const FVector2D RotationVector = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(RotationVector.X * MouseSensitivity.X);
		FRotator NewRotation = Camera->GetComponentRotation();
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + RotationVector.Y * MouseSensitivity.Y, -89.f, 89.f);
		Camera->SetWorldRotation(NewRotation);
	}
}

void AEnemy::P_Reload() 
{
	if (Weapon->GetIsReloadEnabled() && bCanReload && !P_InputManager->IsPressed(FKey("LeftMouseButton")) && Weapon->P_CanReload()) Weapon->P_Reload();
}

void AEnemy::P_ToggleRunning(const bool bState)
{
	if (bCanRun)
	{
		bIsRunning = bState;
		if (bIsRunning) UKismetSystemLibrary::K2_SetTimer(this, "P_TriggerRunningCameraShake", 0.1f, true);
		else UKismetSystemLibrary::K2_ClearTimer(this, "P_TriggerRunningCameraShake");
		if (P_HUD) P_HUD->UpdateRunningIndicator(bState);
		SetMaxWalkSpeed(bState ? P_Data->GetMaxRunSpeed() : P_Data->GetMaxWalkSpeed());
	}
}

void AEnemy::P_OnEnemyDeathEvents()
{
	PossessionBarTimer += 1.f;
	for (int32 i = 0; i < Abilities.Num(); i++)
	{
		if (Abilities[i]->GetIsCooldownKill())
		{
			Abilities[i]->ReduceKillCooldown();
			if (P_HUD) P_HUD->UpdateAbilityCooldown(i, Abilities[i]->GetCurrentCooldownKill());
		}
	}
}

void AEnemy::P_ToggleGodMode()
{
	const TObjectPtr<UGlobalSettingsManager> GlobalSettingsManager = GetWorld()->GetGameInstance()->GetSubsystem<UGlobalSettingsManager>();
	GlobalSettingsManager->ToggleGodModeEnabled();
	P_Data->SetIsGodModeEnabled(GlobalSettingsManager->GetIsGodModeEnabled());
	MaxHp = P_Data->GetMaxHp();
	CurrentHp = MaxHp;
}

void AEnemy::P_UpdateAbilitiesUI()
{
	for (int32 i = 0; i < Abilities.Num(); i++)
	{
		if (!Abilities[i]->GetIsCooldownKill()) P_HUD->UpdateAbilityCooldown(i, Abilities[i]->GetCurrentCooldownSecs());
		else P_HUD->UpdateAbilityCooldown(i, Abilities[i]->GetCurrentCooldownKill());
	}
}

void AEnemy::P_UpdateAbilityCooldownUITextColor(int32 Index)
{
	if (bIsPossessed && !Abilities[Index]->GetIsCooldownKill())
	{
		if (!Abilities[Index]->GetIsAbilityInDuration())
		{
			P_HUD->ChangeColorOfText(Index, FLinearColor::Black);
			P_HUD->UpdateAbilityCooldown(Index, Abilities[Index]->GetCurrentCooldownSecs());
		}
		else
		{
			P_HUD->ChangeColorOfText(Index, FLinearColor::Red);
			P_HUD->UpdateAbilityCooldown(Index, Abilities[Index]->GetCurrentDuration());
		}
	}
}

void AEnemy::P_StopAllAbilities()
{
	for (int32 i = 0; i < Abilities.Num(); i++) Abilities[i]->P_StopAbility();
}

void AEnemy::P_HandleEscapeClick()
{
	switch (GetWorld()->GetSubsystem<UWidgetManager>()->GetCurrentStateOfUI())
	{
		case EWidget::PauseMenuUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->CloseUI(); break;
		case EWidget::SettingsUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->CloseUI(); break;
		default: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->OpenUI();
	}
}

void AEnemy::P_TriggerGlitchingEffect() const
{
	AssetsManager->PlaySound2D(ESound::OnPossession);
	GetWorld()->GetSubsystem<UPostProcessManager>()->TriggerGlitchingMaterial(0.5f);
}

void AEnemy::P_MakeCameraShake(const TSubclassOf<UCameraShakeBase>& InCameraShake) const
{
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(InCameraShake);
}

void AEnemy::P_HandlingDamageTaken()
{
	P_TriggerTakeDamageEffects();
	P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
	if (!bIsCriticallyDamaged && CurrentHp <= MaxHp * 0.3f) P_EnableCriticalDamage();
}

void AEnemy::P_TriggerTakeDamageEffects() const
{
	P_MakeCameraShake(ReceivingDamageCameraShake);
	if (UKismetMathLibrary::RandomInteger(100) >= 75)
	{
		AssetsManager->PlaySound2D(ESound::TakeDamageLayer2);
		GetWorld()->GetSubsystem<UPostProcessManager>()->TriggerGlitchingMaterial(0.25f);
	}
	else AssetsManager->PlaySound2D(ESound::TakeDamageLayer1);
}

void AEnemy::P_EnableCriticalDamage()
{
	UKismetSystemLibrary::K2_SetTimer(this, "P_TriggerCriticalDamageIndicator", 0.5f, true);
	bIsCriticallyDamaged = true;
	P_ToggleCriticallyDamagedEffects(1.f);
	P_HUD->ToggleCriticalDamageIndicator();
	AssetsManager->PlaySound2D(ESound::CriticalHealthEnter);
	AssetsManager->PlaySound2D(ESound::CriticalDamageBeep, true);
}

void AEnemy::P_ToggleCriticallyDamagedEffects(const float InValue) const
{
	GetWorld()->GetSubsystem<UPostProcessManager>()->ToggleMaterial(EPostProcessMaterial::ScreenCrack, InValue);
	GetWorld()->GetSubsystem<UPostProcessManager>()->ToggleMaterial(EPostProcessMaterial::BloodPumping, InValue);
}

void AEnemy::P_InitiateDeathEvents()
{
	if (P_HUD) P_HUD->PlayDeathFadeOutAnim();
	UKismetSystemLibrary::K2_ClearTimer(this, "P_Death");
	GetWorld()->GetSubsystem<UPostProcessManager>()->ToggleMaterial(EPostProcessMaterial::DeathGlitching, 1.f);
	GetWorld()->GetSubsystem<UPostProcessManager>()->ToggleMaterial(EPostProcessMaterial::DeathBloodPumping, 1.f);
	GetWorld()->GetSubsystem<UPostProcessManager>()->ToggleMaterial(EPostProcessMaterial::DeathScreenCrack, 1.f);
	AssetsManager->PlaySound2D(ESound::PlayerDeath);
	AssetsManager->RemoveStoredSound(ESound::CriticalDamageBeep);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, 2.f, FLinearColor::Black, false, true);
	UKismetSystemLibrary::K2_SetTimer(this, "P_OpenDeathMenu", 2.f, false);
}

void AEnemy::P_Death()
{
	bPlayerDied = true;
	if (AssetsManager->IsAsyncLoadingFinished()) P_InitiateDeathEvents();
	else if (!bEnableUnloadLevel)
	{
		bEnableUnloadLevel = true;
		UKismetSystemLibrary::K2_SetTimer(this, "P_Death", 0.01f, true);
	}
}

void AEnemy::P_ControlLogic()
{
	Weapon->P_SetWalkSpread(GetCharacterMovement()->Velocity.Size() >= P_Data->GetMaxWalkSpeed() - 0.1f);
	Weapon->P_SetWalkingSpeed(GetCharacterMovement()->Velocity.Size());
	P_PossessionBarLogic();
}

void AEnemy::P_DetectIfPlayerPointingAtEnemy()
{
	FHitResult HitResult;
	const FVector StartLocation = GetCamera()->GetComponentLocation();
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, StartLocation + GetCamera()->GetForwardVector() * 15000.f, ECC_Visibility))
	{
		if (P_HUD) P_HUD->UpdatePossessionIndicator(HitResult.GetActor()->ActorHasTag("Enemy"));
	}
}

void AEnemy::P_TriggerCriticalDamageIndicator() const
{
	if (P_HUD) P_HUD->ToggleCriticalDamageIndicator();
}

void AEnemy::P_TriggerRunningCameraShake() const { P_MakeCameraShake(RunningCameraShake); }

void AEnemy::P_OpenDeathMenu()
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->ChangeUIToDeathMenu();
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPauseMenuUI>(EWidget::PauseMenuUI)->OpenUI();
}

void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UKismetSystemLibrary::K2_ClearTimer(this, "AI_SetCharacterMovementSpeed");
	UKismetSystemLibrary::K2_ClearTimer(this, "DestroyThis");
	UKismetSystemLibrary::K2_ClearTimer(this, "CheckIfFallOutOfWorld");
	Super::EndPlay(EndPlayReason);
}

void AEnemy::DestroyAI()
{
	if (AI)
	{
		GetWorld()->GetSubsystem<UEnemyCommunicator>()->RemoveEnemy(this);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnXRayThroughWallsChange.RemoveAll(this);
		ChangeXRayState(false);
		AI_StopAllAbilities();
		GetMesh()->SetAnimClass(nullptr);
		AI->DestroyThis();
		AI->UnPossess();
		AI->Destroy();
	}
}

void AEnemy::TickAbilities(float DeltaTime)
{
	for (int32 i = 0; i < Abilities.Num(); i++)
	{
		Abilities[i]->Tick(DeltaTime);
		if (P_HUD) P_UpdateAbilityCooldownUITextColor(i);
	}
}

void AEnemy::ClearTimers()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "P_ControlLogic");
	UKismetSystemLibrary::K2_ClearTimer(this, "P_DetectIfPlayerPointingAtEnemy");
	UKismetSystemLibrary::K2_ClearTimer(this, "P_TriggerCriticalDamageIndicator");
}

void AEnemy::SetupStimuliSource()
{
	StimuliSource = NewObject<UAIPerceptionStimuliSourceComponent>(this);
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->RegisterWithPerceptionSystem();
}

void AEnemy::SetMesh(const ECharacterType PrimaryDataAssetName) const
{
	GetMesh()->SetSkeletalMesh(AssetsManager->GetCharacterData(PrimaryDataAssetName)->CharacterMesh.Get());
}

void AEnemy::P_SetupNavArea()
{
	NavAreaModifier = GetWorld()->SpawnActor<ANavAreaModifier>(P_NavAreaDefaultClass, GetActorLocation(), GetActorRotation());
	NavAreaModifier->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AEnemy::ChangeXRayState(const bool bState) const
{
	GetMesh()->SetRenderCustomDepth(bState);
	GetMesh()->SetCustomDepthStencilValue(bState);
}

void AEnemy::CheckIfFallOutOfWorld() { if (GetActorLocation().Z <= -1000.f) bIsPossessed ? P_Death() : InitiateDeath(); }

void AEnemy::DestroyThis()
{
	Weapon->DestroyThis();
	Destroy();
}
