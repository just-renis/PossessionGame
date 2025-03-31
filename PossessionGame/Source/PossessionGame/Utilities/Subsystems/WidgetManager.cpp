#include "WidgetManager.h"

void UWidgetManager::Init(const TMap<EWidget, TSubclassOf<UUserWidget>>& InWidgetClasses) { WidgetClasses = InWidgetClasses; }

void UWidgetManager::DestroyWidget(const EWidget InWidget)
{
	if (Widgets.Contains(InWidget))
	{
		if (TObjectPtr<UUserWidget> Widget = Widgets[InWidget])
		{
			Widget->RemoveFromParent();
			Widget = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("REMOVED WIDGET"));
			Widgets.Remove(InWidget);
		}
	}
}
