#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "SliderUI.generated.h"

class UProgressBar;
class USlider;

UCLASS()
class POSSESSIONGAME_API USliderUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<USlider> GetSlider() { return Slider; }
	float GetSavedValue() const { return SavedValue; }

	void SaveValue();
	void SetValue(const float InValue);
	void SetSliderValue(const float InValue);
private:
	float SavedValue;
	UPROPERTY(EditAnywhere, Category = "Slider Settings")
	float MinValue = 0.f;
	UPROPERTY(EditAnywhere, Category = "Slider Settings")
	float MaxValue = 1.f;
	UPROPERTY(EditAnywhere, Category = "Slider Bar Settings")
	float BarThickness = 4.f;
	UPROPERTY(EditAnywhere, Category = "Slider Handle Settings")
	FVector2D SliderHandleSize;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<USlider> Slider;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	UFUNCTION()
	void OnSliderValueChanged(const float InValue = 0.f);
};
