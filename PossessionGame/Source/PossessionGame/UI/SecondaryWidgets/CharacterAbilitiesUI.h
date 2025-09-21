#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterAbilitiesUI.generated.h"

struct FAbilityIconData;
class USaveSlot;
enum class ECharacterType : uint8;
class UAbilityIconUI;

UCLASS()
class POSSESSIONGAME_API UCharacterAbilitiesUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UTexture2D> GetAbilityIcon(const int32 InAbilityIndex);
	
	void ResetFirstAbilitySlot();
	void ResetSecondAbilitySlot();
	void LoadAbilitiesLayout(const TObjectPtr<USaveSlot>& InSaveSlot);
	void SaveAbilitiesLayout(const TObjectPtr<USaveSlot>& InSaveSlot);
	void SetAbilitiesIconData();
private:
	UPROPERTY(EditAnywhere, Category = "Character", meta = (AllowPrivateAccess = true))
	ECharacterType Character;
	UPROPERTY(EditAnywhere, Category = "Abilities", meta = (AllowPrivateAccess = true))
	TArray<FAbilityIconData> AbilitiesData;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityIconUI> FirstAbility;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityIconUI> SecondAbility;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityIconUI> ThirdAbility;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityIconUI> FourthAbility;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select1To1Anim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select1To1ReverseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select1To2Anim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select1To2ReverseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select2To1Anim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select2To1ReverseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select2To2Anim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select2To2ReverseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select3To1Anim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select3To1ReverseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select3To2Anim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select3To2ReverseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select4To1Anim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select4To1ReverseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select4To2Anim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Select4To2ReverseAnim;
	UPROPERTY()
	TObjectPtr<UAbilityIconUI> FirstAbilitySlot;
	UPROPERTY()
	TObjectPtr<UAbilityIconUI> SecondAbilitySlot;
	UPROPERTY()
	TArray<TObjectPtr<UAbilityIconUI>> Abilities;

	int32 GetAvailableAbilityIndex();

	virtual void NativeConstruct() override;
	void SetupAbilitySlot(const int32 InAbilityIndex, const bool bIsFirstSlot, TObjectPtr<UAbilityIconUI>& InAbilitySlot);
	void AddAbility(const int32 InAbilityIndex);
	UFUNCTION()
	void FirstAbilitySelected();
	UFUNCTION()
	void SecondAbilitySelected();
	UFUNCTION()
	void ThirdAbilitySelected();
};
