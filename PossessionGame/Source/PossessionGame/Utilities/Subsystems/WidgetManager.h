#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/EWidget.h"
#include "WidgetManager.generated.h"

class UPossessionGameInstance;
enum class EExplanation : uint8;

DECLARE_DELEGATE_TwoParams(FOnShowExplanation, const EExplanation, const TObjectPtr<UPossessionGameInstance>&);

UCLASS()
class POSSESSIONGAME_API UWidgetManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	bool ContainsWidget(const EWidget InWidgetName) const { return Widgets.Contains(InWidgetName); }
	bool GetIsExplanationOpened() const { return bIsExplanationOpened; }
	EWidget GetCurrentStateOfUI() const { return CurrentState; }
	FOnShowExplanation GetOnShowExplanationDelegate() const { return OnShowExplanation; }
	template<typename T>
	TObjectPtr<T> GetWidget(const EWidget InWidgetName);

	void SetCurrentStateOfUI(const EWidget InState) { CurrentState = InState; }
	void Init(const TMap<EWidget, TSubclassOf<UUserWidget>>& InWidgetClasses);
	void DestroyWidget(const EWidget InWidget);
	void SetExplanationState(const bool bState) { bIsExplanationOpened = bState; }
private:
	bool bIsExplanationOpened = false;
	EWidget CurrentState = EWidget::None;
	FOnShowExplanation OnShowExplanation;
	UPROPERTY()
	TMap<EWidget, TSubclassOf<UUserWidget>> WidgetClasses;
	UPROPERTY()
	TMap<EWidget, TObjectPtr<UUserWidget>> Widgets;

	int32 GetWidgetZOrder(const EWidget InWidgetName);
	void ShowExplanation(const EExplanation InExplanationName, const TObjectPtr<UPossessionGameInstance>& InGameInstance);
};

template <typename T>
TObjectPtr<T> UWidgetManager::GetWidget(const EWidget InWidgetName)
{
	if (Widgets.Contains(InWidgetName)) return Cast<T>(Widgets[InWidgetName]);
	Widgets.Add(InWidgetName, CreateWidget<T>(GetWorld(), WidgetClasses[InWidgetName]));
	Widgets[InWidgetName]->AddToViewport(GetWidgetZOrder(InWidgetName));
	return Cast<T>(Widgets[InWidgetName]);
}