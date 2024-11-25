#include "PossessionGameUserSettings.h"

void UPossessionGameUserSettings::SetGamma(float InGammaValue)
{
    GammaValue = InGammaValue;
	static IConsoleVariable* TonemapperGammaCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TonemapperGamma"));
	if (TonemapperGammaCVar) TonemapperGammaCVar->Set(GammaValue);
}

void UPossessionGameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	SetGamma(GammaValue);
}
