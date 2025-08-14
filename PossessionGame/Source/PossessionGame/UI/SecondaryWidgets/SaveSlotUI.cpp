#include "SaveSlotUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/MetaSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void USaveSlotUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (SaveSlotText) SetText();
	SaveSlotButton->OnClicked.AddDynamic(this, &USaveSlotUI::OnClicked);
	SaveSlotButton->OnHovered.AddDynamic(this, &USaveSlotUI::OnHovered);
	SaveSlotButton->OnUnhovered.AddDynamic(this, &USaveSlotUI::OnUnHovered);
	
}

void USaveSlotUI::SetText() const
{
	if (GetGameInstance()->GetSubsystem<USaveSlotManager>()->DoesSaveSlotExists(SlotName))
	{
		const TObjectPtr<UMetaSaveSlot> MetaSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetMetaSaveSlot();
		SaveSlotText->SetText(MetaSaveSlot->GetSaveSlotMetaDataText(SlotName));
	}
	else SaveSlotText->SetText(NSLOCTEXT("ST_Localization", "EmptySlot", "Empty Slot"));
}

void USaveSlotUI::OnClicked()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ClickArrow);
	PlayAnimation(SelectAnim);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSaveSlotClicked.ExecuteIfBound(SlotName, RunSlotName);
}

void USaveSlotUI::OnHovered()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	PlayAnimationForward(HoverAnim);
}

void USaveSlotUI::OnUnHovered() { PlayAnimationReverse(HoverAnim); }
