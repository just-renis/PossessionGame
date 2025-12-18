#include "CustomEditableText.h"

void UCustomEditableText::Init() { OnTextChanged.AddDynamic(this, &UCustomEditableText::OnTextChangedFiltered); }

void UCustomEditableText::OnTextChangedFiltered(const FText& InText)
{
	FString Input = InText.ToString();
	FString Filtered;
	for (const TCHAR Char : Input) if (FChar::IsDigit(Char)) Filtered.AppendChar(Char);
	if (Filtered.Len() > 3) Filtered.LeftInline(3);
	if (Filtered != Input) SetText(FText::FromString(Filtered));
}
