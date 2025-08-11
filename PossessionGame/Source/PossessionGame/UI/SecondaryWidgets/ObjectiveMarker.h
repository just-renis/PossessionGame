#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveMarker.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UObjectiveMarker : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(const bool bIsTextNeeded) const;
	void UpdateMarkerText(const int32 InMeters) const;
	void SetMarkerImage(const TObjectPtr<UTexture2D>& InTexture) const;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MarkerText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MarkerImage;
};
