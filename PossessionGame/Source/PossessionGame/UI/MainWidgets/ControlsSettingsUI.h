#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlsSettingsUI.generated.h"

class USettingsUI;
struct FPlayerKeyMapping;
class UEnhancedInputUserSettings;
class UVerticalBox;
class UInputMappingContext;
class UKeyBindingUI;

UCLASS()
class POSSESSIONGAME_API UControlsSettingsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	bool ApplySettings();
	
	void SetSavedSettings();
	void ResetKeyBindsToDefault();
	void Setup(const TObjectPtr<USettingsUI>& InParent);
private:
	int32 DesiredOrderMovement = 6;
	int32 DesiredOrderAction = 13;
	UPROPERTY(EditDefaultsOnly, Category = "Key Binds", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> LineSeparatorTexture;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> SettingsTitleClass;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> KeyBindingClass;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UVerticalBox> KeyBindingsPanel;
	UPROPERTY(EditAnywhere, Category = "Mapping Context", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> MappingContext;
	UPROPERTY()
	TObjectPtr<UEnhancedInputUserSettings> UserSettings;
	UPROPERTY()
	TObjectPtr<USettingsUI> ParentWidget;
	UPROPERTY()
	TArray<TObjectPtr<UKeyBindingUI>> KeyBindings;

	bool IsKeyBindsHasDuplicates();

	virtual void NativeConstruct() override;
	void SetupKeyBinds();
	void SetupOrderOfKeyBinds(TArray<FName>& DesiredOrder, TMap<FName, FPlayerKeyMapping>& OrderedKeyBinds);
	void AddKeyBinds(TArray<FName>& DesiredOrder, TMap<FName, FPlayerKeyMapping>& OrderedKeyBinds);
};
