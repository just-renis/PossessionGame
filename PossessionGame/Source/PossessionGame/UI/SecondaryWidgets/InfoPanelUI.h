#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfoPanelUI.generated.h"

class UInfoAbilityDetailsUI;
enum class ECharacterType : uint8;
class UImage;

USTRUCT()
struct FAbilityIconAndDescription
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (MultiLine))
	FText Description;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;
};

USTRUCT()
struct FInfoPanelData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FAbilityIconAndDescription> AbilitiesData;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CharacterIcon;
};

UCLASS()
class POSSESSIONGAME_API UInfoPanelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void Setup(const ECharacterType InCharacter) const;
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<ECharacterType, FInfoPanelData> Data;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInfoAbilityDetailsUI> FirstAbilityDetails;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInfoAbilityDetailsUI> SecondAbilityDetails;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInfoAbilityDetailsUI> ThirdAbilityDetails;
	UPROPERTY()
	TArray<TObjectPtr<UInfoAbilityDetailsUI>> AbilityDetailWidgets;

	virtual void NativeConstruct() override;
};
