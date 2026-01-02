#include "PlayerLevelOptionUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void UPlayerLevelOptionUI::Setup(const bool bIsUnlocked, const TObjectPtr<UPlayerLevelOptionUI>& InOtherOptionWidget, const EPlayerLevelUpgradeId InId, const EPlayerLevelUpgradeId InOtherOptionId, const FText& InText, const bool bIsChosen, const TObjectPtr<UObject>& InOptionIconTexture)
{
	Id = InId;
	OtherOptionId = InOtherOptionId;
	OptionText->SetText(bIsUnlocked ? InText : NSLOCTEXT("ST_Localization", "Locked", "<BigUpgradeAttributeText>Locked</>"));
	OptionIcon->SetBrushFromAtlasInterface(InOptionIconTexture);
	if (!bIsUnlocked)
	{
		OptionButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return;
	}
	if (bIsChosen) SetAsChosen();
	OptionButton->OnClicked.AddDynamic(InOtherOptionWidget, &UPlayerLevelOptionUI::OnOtherOptionSelected);
}

void UPlayerLevelOptionUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!OptionButton) return;
	NotSelectedTexture = OptionButton->GetStyle().Normal.GetResourceObject();
	OptionButton->OnClicked.AddDynamic(this, &UPlayerLevelOptionUI::OnSelected);
	OptionButton->OnHovered.AddDynamic(this, &UPlayerLevelOptionUI::OnHoverTrigger);
	OptionButton->OnUnhovered.AddDynamic(this, &UPlayerLevelOptionUI::OnHoverTrigger);
}

void UPlayerLevelOptionUI::SetAsChosen() const
{
	FButtonStyle ButtonStyle = OptionButton->GetStyle();
	ButtonStyle.Normal.SetResourceObject(ButtonStyle.Disabled.GetResourceObject());
	OptionButton->SetStyle(ButtonStyle);
	OptionButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerLevelOptionUI::OnSelected()
{
	if (!OptionButton || OptionButton->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) return;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ClickArrow);
	SetAsChosen();
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->ChangePlayerLevelUpgrade(Id, OtherOptionId);
}

void UPlayerLevelOptionUI::OnOtherOptionSelected()
{
	FButtonStyle ButtonStyle = OptionButton->GetStyle();
	ButtonStyle.Normal.SetResourceObject(NotSelectedTexture);
	OptionButton->SetStyle(ButtonStyle);
	OptionButton->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerLevelOptionUI::OnHoverTrigger()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
}
