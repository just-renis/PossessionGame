#include "InfoPanelUI.h"
#include "InfoAbilityDetailsUI.h"
#include "Components/Image.h"
#include "PossessionGame/Enums/ECharacterType.h"

void UInfoPanelUI::Setup(const ECharacterType InCharacter) const
{
	if (!CharacterIcon || !FirstAbilityDetails || !SecondAbilityDetails || !ThirdAbilityDetails) return;
	if (const FInfoPanelData* InfoPanelData = Data.Find(InCharacter))
	{
		CharacterIcon->SetBrushFromAtlasInterface(InfoPanelData->CharacterIcon);
		for (int32 i = 0; i < AbilityDetailWidgets.Num(); i++)
		{
			if (!AbilityDetailWidgets.IsValidIndex(i)) continue;
			if (InfoPanelData->AbilitiesData.IsValidIndex(i))
			{
				AbilityDetailWidgets[i]->SetText(InfoPanelData->AbilitiesData[i].Description);
				AbilityDetailWidgets[i]->SetAbilityIcon(InfoPanelData->AbilitiesData[i].Icon);
				AbilityDetailWidgets[i]->SetAbilityVisibility(true);
			}
			else AbilityDetailWidgets[i]->SetAbilityVisibility(false);
		}
	}
}

void UInfoPanelUI::NativeConstruct()
{
	Super::NativeConstruct();
	AbilityDetailWidgets = { FirstAbilityDetails, SecondAbilityDetails, ThirdAbilityDetails };
}
