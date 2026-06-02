#include "DestructibleData.h"
#include "PossessionGame/Utilities/Data Utilities/FVariationVfxParams.h"

void UDestructibleData::SetupVariationMaterial(const TObjectPtr<UMaterialInstanceDynamic>& InMaterial, const EVariation InVariation)
{
	if (const FVariationVfxParams* Data = VariationValues.Find(InVariation))
	{
		for (const TPair<FName, FNiagaraUserParam>& Param : Data->Params)
		{
			switch (Param.Value.ParamType)
			{
			case EUserParamType::Bool: InMaterial->SetScalarParameterValue(Param.Key, Param.Value.bBoolValue); break;
			case EUserParamType::Int: InMaterial->SetScalarParameterValue(Param.Key, Param.Value.IntValue); break;
			case EUserParamType::Float: InMaterial->SetScalarParameterValue(Param.Key, Param.Value.FloatValue); break;
			case EUserParamType::Vector: InMaterial->SetVectorParameterValue(Param.Key, Param.Value.VectorValue); break;
			case EUserParamType::Color: InMaterial->SetVectorParameterValue(Param.Key, Param.Value.ColorValue); break;
			case EUserParamType::Texture: InMaterial->SetTextureParameterValue(Param.Key, Param.Value.Texture); break;
			default: ;
			}
		}
	}
}
