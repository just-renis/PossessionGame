#include "WidgetManager.h"

void UWidgetManager::Init(TMap<EWidget, TSubclassOf<UUserWidget>>& InWidgetClasses)
{
	WidgetClasses = InWidgetClasses;
}
