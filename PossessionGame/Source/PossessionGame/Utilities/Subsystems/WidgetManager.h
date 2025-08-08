#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/EWidget.h"
#include "WidgetManager.generated.h"

class UDelegateManager;
enum class EExplanation : uint8;

UCLASS()
class POSSESSIONGAME_API UWidgetManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	bool ContainsWidget(const EWidget InWidgetName) const { return Widgets.Contains(InWidgetName); }
	EWidget GetCurrentStateOfUI() const { return CurrentState; }
	template<typename T>
	TObjectPtr<T> GetWidget(const EWidget InWidgetName);

	void SetCurrentStateOfUI(const EWidget InState) { CurrentState = InState; }
	void Init(const TMap<EWidget, TSubclassOf<UUserWidget>>& InWidgetClasses, const bool bIsTutorialFinished);
	void DestroyWidget(const EWidget InWidget);
private:
	EWidget CurrentState = EWidget::None;
	UPROPERTY()
	TMap<EWidget, TSubclassOf<UUserWidget>> WidgetClasses;
	UPROPERTY()
	TMap<EWidget, TObjectPtr<UUserWidget>> Widgets;

	int32 GetWidgetZOrder(const EWidget InWidgetName);
	void ShowExplanation(const EExplanation InExplanationName);
};

template <typename T>
TObjectPtr<T> UWidgetManager::GetWidget(const EWidget InWidgetName)
{
	if (Widgets.Contains(InWidgetName)) return Cast<T>(Widgets[InWidgetName]);
	Widgets.Add(InWidgetName, CreateWidget<T>(GetWorld(), WidgetClasses[InWidgetName]));
	Widgets[InWidgetName]->AddToViewport(GetWidgetZOrder(InWidgetName));
	return Cast<T>(Widgets[InWidgetName]);
}