#include "SliderUI.h"
#include "Components/ProgressBar.h"

void USliderUI::SaveValue() { SavedValue = Slider->GetValue(); }

void USliderUI::SetValue(float InValue)
{
	SavedValue = InValue;
	Slider->SetValue(InValue);
}

void USliderUI::SetSliderValue(float InValue)
{
	Slider->SetValue(InValue);
}

void USliderUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (Slider)
	{
		Slider->OnValueChanged.AddDynamic(this, &USliderUI::OnSliderValueChanged);
		if (ProgressBar) ProgressBar->SetPercent(Slider->GetValue());
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
		if (ProgressBar) ProgressBar->SetRenderScale(FVector2D(1.f, BarThickness / 4.f));
	}
}

void USliderUI::OnSliderValueChanged(float Value) { if (ProgressBar) ProgressBar->SetPercent(Value); }
