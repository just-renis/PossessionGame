#pragma once

#include "CoreMinimal.h"
#include "Destructible.h"
#include "RangerShield.generated.h"

enum class ESound : uint8;

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
	TObjectPtr<AActor> Proprietor;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> M_ForceField;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> M_Shield;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	virtual void BeginPlay() override;
	virtual void Init(bool bInIsPlayerObject) override;
	virtual void DestroyThis() override;
	void UpdateShieldMaterialValues(const float InOpacity, const float InProgress) const;
	void PlaySound(const ESound InSoundName);
	UFUNCTION()
	void MoveShield();
	UFUNCTION()
	void DissolveShield();
};
