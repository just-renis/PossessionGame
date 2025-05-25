#pragma once

#include "CoreMinimal.h"
#include "SettingsOptionUI.h"
#include "SettingsQualityUI.generated.h"

enum class EGraphicsQuality : uint8;

UCLASS()
class POSSESSIONGAME_API USettingsQualityUI : public USettingsOptionUI
{
	GENERATED_BODY()
public:
	EGraphicsQuality GetQuality() const { return AppliedQuality; }
	EGraphicsQuality SetAndGetAppliedQuality();
	
	void UpdateQuality(const uint8 InQualityInt);
	void UpdateDefaultQuality(const uint8 InQualityInt);
	void UpdateQuality();
private:
	EGraphicsQuality TemporalQuality;
	EGraphicsQuality AppliedQuality;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool IsGlobalGraphicsSetting = false;

	EGraphicsQuality GetQualityEnum(const uint8 InValue);

	void SetGlobalGraphicsSettingToCustom()  { UpdateQuality(5); }
	virtual void NativeConstruct() override;
	virtual void OnLeftArrowClicked() override;
	virtual void OnRightArrowClicked() override;
};
