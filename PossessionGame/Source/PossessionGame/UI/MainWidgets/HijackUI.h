#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HijackUI.generated.h"

class UImage;

UCLASS()
class POSSESSIONGAME_API UHijackUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void TriggerHijackProgress();
	void OpenUI(const float InHijackAnimSpeed);
	void CloseUI() const;
private:
	float Progress = 0.f;
	float HijackSpeedMultiplier = 1.f;
	FTimerHandle HijackHandle;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HijackRadial;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> HijackInv;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HijackInitiateAnim;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_HijackRadial;

	virtual void NativeConstruct() override;
	void HijackProgress();
};
