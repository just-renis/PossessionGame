#include "WidgetManager.h"

void UWidgetManager::Init(const TMap<EWidget, TSubclassOf<UUserWidget>>& InWidgetClasses) { WidgetClasses = InWidgetClasses; }
