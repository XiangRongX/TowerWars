// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/TowerDataAsset.h"

int32 UTowerDataAsset::GetUnlockIncomeRequirement() const
{
	return FMath::CeilToInt(Cost / 5.0f);
}

bool UTowerDataAsset::IsUnlocked(int32 PlayerIncome) const
{
	if (bDefaultUnlocked) return true;
	return PlayerIncome >= GetUnlockIncomeRequirement();
}
