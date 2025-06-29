#include "LoadingScreenUI.h"
#include "ShaderPipelineCache.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/FTFunctionCallback.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void ULoadingScreenUI::Init(const TFunction<void()>& InCallback)
{
	PlayFadeInAnim();
	TotalPreCompiles = FShaderPipelineCache::NumPrecompilesRemaining();
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, "LoadingProcess", InCallback);
	GetWorld()->GetTimerManager().SetTimer(PSOPrecachingHandle, TimerDel, 0.1f, true);
}

void ULoadingScreenUI::NativeConstruct()
{
	Super::NativeConstruct();
	FadeOutFinishedAnimDelegate.BindUFunction(this, "DestroyThis");
}

void ULoadingScreenUI::PlayFadeInAnim() { PlayAnimation(FadeInAnim); }

void ULoadingScreenUI::PlayFadeOutAnim()
{
	StopAnimation(FadeInAnim);
	PlayAnimation(FadeOutAnim);
	BindToAnimationFinished(FadeOutAnim, FadeOutFinishedAnimDelegate);
}

void ULoadingScreenUI::DestroyThis()
{
	UnbindFromAnimationFinished(FadeOutAnim, FadeOutFinishedAnimDelegate);
	GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::LoadingScreenUI);
}

void ULoadingScreenUI::LoadingProcess(const FTFunctionCallback& InCallback)
{
	const int32 Remaining = FShaderPipelineCache::NumPrecompilesRemaining();
	if (Remaining <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(PSOPrecachingHandle);
		if (InCallback.Function) InCallback.Function();
		PlayFadeOutAnim();
	}
	else
	{
		const float ProgressPercent = (1.f - Remaining / static_cast<float>(TotalPreCompiles)) * 100.f;
		if (LoadingBarText) LoadingBarText->SetText(FText::Format(FText::FromString("{0}%"), FMath::RoundToInt(ProgressPercent)));
		if (LoadingBar) LoadingBar->SetPercent(ProgressPercent * 0.01f);
		if (ShadersText) ShadersText->SetText(FText::Format(FText::FromString("{0} / {1} Shaders Compiled"), TotalPreCompiles - Remaining, TotalPreCompiles));
	}
}
