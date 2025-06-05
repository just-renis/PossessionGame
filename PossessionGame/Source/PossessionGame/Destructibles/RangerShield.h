#pragma once

#include "CoreMinimal.h"
#include "Destructible.h"
#include "RangerShield.generated.h"

enum class ESound : uint16;

UCLASS()
class POSSESSIONGAME_API ARangerShield : public ADestructible
{
	GENERATED_BODY()
public:
	ARangerShield();
	
	void SetProprietor(const TObjectPtr<AActor>& InProprietor) { Proprietor = InProprietor; }
private:
	int32 ShieldMaterialValueChanger;
	UPROPERTY()
	FTimerHandle DissolveHandle;
	UPROPERTY()
	FTimerHandle MoveShieldHandle;
	UPROPERTY()
	TObjectPtr<AActor> Proprietor;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> M_ForceField;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> M_Shield;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

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
};
