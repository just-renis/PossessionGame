#include "DestructibleData.h"
#include "PossessionGame/Utilities/Data Utilities/FVariationVfxParams.h"

void UDestructibleData::SetupVariationMaterials(const TArray<TObjectPtr<UMaterialInstanceDynamic>>& InMaterials, const EVariation InVariation)
{
	const FMaterialVariationVfxParams* VariationData = VariationValues.Find(InVariation);
	if (!VariationData) return;
	for (const TPair<int32, FVariationVfxParams>& MaterialPair : VariationData->Params)
	{
		const int32 MaterialIndex = MaterialPair.Key;
		if (!InMaterials.IsValidIndex(MaterialIndex)) continue;
		UMaterialInstanceDynamic* Material = InMaterials[MaterialIndex];
		for (const TPair<FName, FNiagaraUserParam>& Param : MaterialPair.Value.Params)
		{
			switch (Param.Value.ParamType)
			{
				case EUserParamType::Bool: Material->SetScalarParameterValue(Param.Key, Param.Value.bBoolValue); break;
				case EUserParamType::Int: Material->SetScalarParameterValue(Param.Key, Param.Value.IntValue); break;
				case EUserParamType::Float: Material->SetScalarParameterValue(Param.Key, Param.Value.FloatValue); break;
				case EUserParamType::Vector: Material->SetVectorParameterValue(Param.Key, Param.Value.VectorValue); break;
				case EUserParamType::Color: Material->SetVectorParameterValue(Param.Key, Param.Value.ColorValue); break;
				case EUserParamType::Texture: Material->SetTextureParameterValue(Param.Key, Param.Value.Texture); break;
				default: break;
			}
		}
	}
}
