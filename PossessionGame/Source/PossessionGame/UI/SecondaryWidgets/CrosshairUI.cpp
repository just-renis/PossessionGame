#include "CrosshairUI.h"
#include "Components/Image.h"

void UCrosshairUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (CrosshairTexture && CrosshairImage) CrosshairImage->SetBrushFromAtlasInterface(CrosshairTexture);
}
