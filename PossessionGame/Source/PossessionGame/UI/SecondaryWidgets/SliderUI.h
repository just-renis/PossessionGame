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
	void SetValue(float InValue);
	void SetSliderValue(float InValue);
private:
	float SavedValue;
	UPROPERTY(EditAnywhere, Category = "Slider Bar Settings")
	float BarThickness;
	UPROPERTY(EditAnywhere, Category = "Slider Handle Settings")
	FVector2D SliderHandleSize;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<USlider> Slider;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	UFUNCTION()
	void OnSliderValueChanged(float Value);
};
