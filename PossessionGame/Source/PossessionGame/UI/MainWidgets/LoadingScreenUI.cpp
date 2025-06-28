#include "LoadingScreenUI.h"
#include "ShaderPipelineCache.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void ULoadingScreenUI::Init(const TFunction<void()>& InCallback)
{
	TotalPreCompiles = FShaderPipelineCache::NumPrecompilesRemaining();
	GetWorld()->GetTimerManager().SetTimer(PSOPrecachingHandle, [this, InCallback]
	{
		const int32 Remaining = FShaderPipelineCache::NumPrecompilesRemaining();
		if (Remaining <= 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(PSOPrecachingHandle);
			if (InCallback) InCallback();
			SetDestroyTimer();
		}
		else
		{
			const float ProgressPercent = Remaining / TotalPreCompiles * 100.f;
			if (LoadingBarText) LoadingBarText->SetText(FText::Format(FText::FromString("{0}%"), ProgressPercent));
			if (LoadingBar) LoadingBar->SetPercent(ProgressPercent * 0.01f);
			UE_LOG(LogTemp, Log, TEXT("MainMenuLevel Remaining PSO Precompiles: %d"), Remaining);
		}
	}, 0.1f, true);
}

void ULoadingScreenUI::SetDestroyTimer()
{
	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &ULoadingScreenUI::DestroyThis, 0.1f);
}

void ULoadingScreenUI::DestroyThis() const
{
	GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::LoadingScreenUI);
}
