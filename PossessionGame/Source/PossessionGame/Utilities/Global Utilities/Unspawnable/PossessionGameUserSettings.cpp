#include "PossessionGameUserSettings.h"

void UPossessionGameUserSettings::SetMotionBlur(const bool bState)
{
	bIsMotionBlurEnabled = bState;
	static IConsoleVariable* MotionBlurCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality"));
	if (MotionBlurCVar) MotionBlurCVar->Set(bIsMotionBlurEnabled ? 3 : 0);
}

void UPossessionGameUserSettings::SetGamma(const float InGammaValue)
{
    GammaValue = InGammaValue;
	static IConsoleVariable* TonemapperGammaCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TonemapperGamma"));
	if (TonemapperGammaCVar) TonemapperGammaCVar->Set(GammaValue);
}

void UPossessionGameUserSettings::SetAntiAliasingMethod(const int32 InAntiAliasingMethod)
{
	AntiAliasingMethod = InAntiAliasingMethod;
	static IConsoleVariable* AntiAliasingMethodCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
	if (AntiAliasingMethodCVar) AntiAliasingMethodCVar->Set(AntiAliasingMethod);
}

void UPossessionGameUserSettings::LoadSettings(const bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	SetGamma(GammaValue);
	SetAntiAliasingMethod(AntiAliasingMethod);
}
