#include "PossessionGameUserSettings.h"

void UPossessionGameUserSettings::SetGamma(const float InGammaValue)
{
    GammaValue = InGammaValue;
	static IConsoleVariable* TonemapperGammaCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TonemapperGamma"));
	if (TonemapperGammaCVar) TonemapperGammaCVar->Set(GammaValue);
}

void UPossessionGameUserSettings::SetAntiAliasingMethod(const int32 InAntiAliasingMethod)
{
	AntiAliasingMethod = InAntiAliasingMethod;
	static IConsoleVariable* AntiAliasingMethodConsoleVariable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
	if (AntiAliasingMethodConsoleVariable)
	{
		UE_LOG(LogTemp, Error, TEXT("ANTIALIASING METHOD IS SET TO: %i"), AntiAliasingMethod);
		AntiAliasingMethodConsoleVariable->Set(AntiAliasingMethod);
	}
}

void UPossessionGameUserSettings::LoadSettings(const bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	SetGamma(GammaValue);
	SetAntiAliasingMethod(AntiAliasingMethod);
}
