#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsQualityUI.generated.h"

class UTextBlock;
enum class EGraphicsQuality : uint8;
class UButton;

UCLASS()
class POSSESSIONGAME_API USettingsQualityUI : public UUserWidget
{
	GENERATED_BODY()
public:
	EGraphicsQuality GetQuality() { return AppliedQuality; }
	EGraphicsQuality SetAndGetAppliedQuality();
	
	void UpdateQuality(uint8 QualityInt);
	void UpdateDefaultQuality(uint8 QualityInt);
	void UpdateQuality();
private:
	EGraphicsQuality TemporalQuality;
	EGraphicsQuality AppliedQuality;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool IsGlobalGraphicsSetting = false;
	UPROPERTY(EditAnywhere, Category = "Quality Text Font Size", meta = (AllowPrivateAccess = true))
	int32 FontSize = 24;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> SettingQualityText;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> SettingButtonDownscale;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> SettingButtonUpscale;

	EGraphicsQuality GetQualityEnum(uint8 Value);

	void SetGlobalGraphicsSettingToCustom()  { UpdateQuality(5); }
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	void SetupButtonEvents();
	UFUNCTION()
	virtual void LowerQuality();
	UFUNCTION()
	virtual void IncreaseQuality();
};
