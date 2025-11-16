#include "CodexData.h"
#include "CharacterData.h"

TObjectPtr<UCharacterData> UCodexData::GetCharacterData(const bool bIsPlayer, const ECharacterType InCharacterName)
{
	if (bIsPlayer)
	{
		if (const TObjectPtr<UCharacterData>* Data = PlayerCharactersHp.Find(InCharacterName)) return *Data;
	}
	else
	{
		if (const TObjectPtr<UCharacterData>* Data = EnemyCharactersHp.Find(InCharacterName)) return *Data;
	}
	return nullptr;
}

TObjectPtr<UAbilityData> UCodexData::GetAbilityData(const bool bIsPlayer, const EAbility InAbilityName)
{
	if (bIsPlayer)
	{
		if (const TObjectPtr<UAbilityData>* Data = PlayerAbilitiesData.Find(InAbilityName)) return *Data;
	}
	else
	{
		if (const TObjectPtr<UAbilityData>* Data = EnemyAbilitiesData.Find(InAbilityName)) return *Data;
	}
	return nullptr;
}

TObjectPtr<UDestructibleData> UCodexData::GetDestructibleData(const bool bIsPlayer, const EPoolObject InDestructibleName)
{
	if (bIsPlayer)
	{
		if (const TObjectPtr<UDestructibleData>* Data = PlayerDestructiblesData.Find(InDestructibleName)) return *Data;
	}
	else
	{
		if (const TObjectPtr<UDestructibleData>* Data = EnemyDestructiblesData.Find(InDestructibleName)) return *Data;
	}
	return nullptr;
}

TObjectPtr<UThrowableData> UCodexData::GetThrowableData(const bool bIsPlayer, const EPoolObject InThrowableName)
{
	if (bIsPlayer)
	{
		if (const TObjectPtr<UThrowableData>* Data = PlayerThrowablesData.Find(InThrowableName)) return *Data;
	}
	else
	{
		if (const TObjectPtr<UThrowableData>* Data = EnemyThrowablesData.Find(InThrowableName)) return *Data;
	}
	return nullptr;
}

TObjectPtr<UCodexProjectileData> UCodexData::GetProjectileData(const bool bIsPlayer, const EPoolObject InProjectileName)
{
	if (bIsPlayer)
	{
		if (const TObjectPtr<UCodexProjectileData>* Data = PlayerProjectilesData.Find(InProjectileName)) return *Data;
	}
	else
	{
		if (const TObjectPtr<UCodexProjectileData>* Data = EnemyProjectilesData.Find(InProjectileName)) return *Data;
	}
	return nullptr;
}
