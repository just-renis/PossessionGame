#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/EWidget.h"
#include "WidgetManager.generated.h"

UCLASS()
class POSSESSIONGAME_API UWidgetManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	EWidget GetCurrentStateOfUI() { return CurrentState; }
	template<typename T>
	TObjectPtr<T> GetWidget(EWidget WidgetName);

	void SetCurrentStateOfUI(EWidget State) { CurrentState = State; }
	void Init(TMap<EWidget, TSubclassOf<UUserWidget>>& InWidgetClasses);
private:
	EWidget CurrentState = EWidget::None;
	UPROPERTY()
	TMap<EWidget, TSubclassOf<UUserWidget>> WidgetClasses;
	UPROPERTY()
	TMap<EWidget, TObjectPtr<UUserWidget>> Widgets;
};

template <typename T>
TObjectPtr<T> UWidgetManager::GetWidget(EWidget WidgetName)
{
	if (Widgets.Contains(WidgetName)) return Cast<T>(Widgets[WidgetName]);
	Widgets.Add(WidgetName, CreateWidget<T>(GetWorld(), WidgetClasses[WidgetName]));
	Widgets[WidgetName]->AddToViewport();
	return Cast<T>(Widgets[WidgetName]);
}