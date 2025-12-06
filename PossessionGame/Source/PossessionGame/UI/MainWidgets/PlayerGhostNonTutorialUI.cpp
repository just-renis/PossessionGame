#include "PlayerGhostNonTutorialUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UPlayerGhostNonTutorialUI::NativeConstruct()
{
	Super::NativeConstruct();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnKeyBindChange.AddUObject(this, &UPlayerGhostNonTutorialUI::UpdateHijackKeyBindText);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnPlayerGhostDestroy.BindUObject(this, &UPlayerGhostNonTutorialUI::DestroyThis);
	UpdateHijackKeyBindText(FName("Hijack"), GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText("Hijack"));
}

void UPlayerGhostNonTutorialUI::UpdateHijackKeyBindText(const FName& InKeyBindName, const FText& InKeyBindText) const
{
	if (InKeyBindName.IsEqual("Hijack")) HijackHintText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "HijackRobotsHint", "Look at Robot and Press [{0}] to Hijack"), InKeyBindText));
}

void UPlayerGhostNonTutorialUI::NativeDestruct()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnKeyBindChange.RemoveAll(this);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnPlayerGhostDestroy.Unbind();
	Super::NativeDestruct();
}

void UPlayerGhostNonTutorialUI::DestroyThis() const { GetWorld()->GetSubsystem<UWidgetManager>()->DestroyWidget(EWidget::PlayerGhostNonTutorialUI); }
