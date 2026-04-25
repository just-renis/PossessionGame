#pragma once

#include "CoreMinimal.h"
#include "Destructible.h"
#include "RangerShield.generated.h"

class UAIHpBarUI;
class UWidgetComponent;
class AEnemy;
enum class ESound : uint16;

UCLASS()
class POSSESSIONGAME_API ARangerShield : public ADestructible
{
	GENERATED_BODY()
public:
	ARangerShield();
	
	void SetProprietor(const TObjectPtr<AEnemy>& InProprietor) { Proprietor = InProprietor; }
private:
	bool bHasBeenDamagedRecently = false;
	int32 ShieldMaterialValueChanger;
	FTimerHandle DissolveHandle;
	FTimerHandle MoveShieldHandle;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> HpBarComponent;
	UPROPERTY()
	TObjectPtr<UAIHpBarUI> HpBar;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_ForceField;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Shield;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Init(bool bInIsPlayerObject) override;
	virtual void DestroyThis() override;
	void UpdateShieldMaterialValues(const float InOpacity, const float InProgress) const;
	void PlaySound(const ESound InSoundName);
	void MoveShield();
	void DissolveShield();
	virtual void SetThisLifeSpan() override;
	virtual void SetUpgradesApplication() override;
	void SetupHpBar();
	void DestroyHpBar();
	void HideHpBar() const;
	void ShowHpBar() const;
	virtual void SubtractHp(const float InDamage) override;
	void DisableDamageTakenRedFlash() const;
};
