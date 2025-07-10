#include "SliderUI.h"
#include "Components/ProgressBar.h"

void USliderUI::SaveValue() { if (Slider) SavedValue = Slider->GetValue(); }

void USliderUI::SetValue(const float InValue)
{
	SavedValue = InValue;
	if (Slider) Slider->SetValue(InValue);
}

void USliderUI::SetSliderValue(const float InValue) { if (Slider) Slider->SetValue(InValue); }

void USliderUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (Slider)
	{
		Slider->OnValueChanged.AddDynamic(this, &USliderUI::OnSliderValueChanged);
		OnSliderValueChanged();
	}
}

void USliderUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Slider)
	{
		FSliderStyle Style = Slider->GetWidgetStyle();
		Style.NormalThumbImage.ImageSize = Style.HoveredThumbImage.ImageSize = Style.DisabledThumbImage.ImageSize = SliderHandleSize;
		Style.BarThickness = BarThickness;
		Slider->SetWidgetStyle(Style);
		Slider->SetMinValue(MinValue);
		Slider->SetMaxValue(MaxValue);
		if (ProgressBar) ProgressBar->SetRenderScale(FVector2D(1.f, BarThickness * 0.25f));
	}
}

void USliderUI::OnSliderValueChanged(float Value) { if (ProgressBar) ProgressBar->SetPercent((Slider->GetValue() - MinValue) / (MaxValue - MinValue)); }
