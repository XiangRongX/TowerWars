// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/EnemyDataAsset.h"

int32 UEnemyDataAsset::GetCostForStar(int32 StarLevel) const
{
	int32 ClampedStar = FMath::Clamp(StarLevel, 0, 5);
	float CalculatedCost = Cost * FMath::Pow(1.7f, static_cast<float>(ClampedStar));
	return FMath::CeilToInt(CalculatedCost);
}

int32 UEnemyDataAsset::GetIncomeForStar(int32 StarLevel) const
{
	int32 ClampedStar = FMath::Clamp(StarLevel, 0, 5);
	float CalculatedIncome = Income * FMath::Pow(1.2f, static_cast<float>(ClampedStar));
	return FMath::CeilToInt(CalculatedIncome);
}

int32 UEnemyDataAsset::GetUnlockIncomeRequirement() const
{
	if (bDefaultUnlocked)
	{
		return 0;
	}

	if (!bIsUpgradedEnemy)
	{
		// 基础怪物：0星 Cost * 0.2
		return FMath::CeilToInt(Cost * 0.2f);
	}

	// 进阶怪物：0星 Cost * 1.0
	return Cost;
}

int32 UEnemyDataAsset::GetStarIncomeRequirement(int32 TargetStar) const
{
	if (!bIsUpgradedEnemy || TargetStar < 1 || TargetStar > 5)
	{
		return 0;
	}

	// 1星基础要求：0星 Cost * 16
	float Requirement = Cost * 16.0f;

	// 2~5星要求：在前一星级要求的基础上 * 1.7
	if (TargetStar > 1)
	{
		Requirement *= FMath::Pow(1.7f, static_cast<float>(TargetStar - 1));
	}

	return FMath::CeilToInt(Requirement);
}

int32 UEnemyDataAsset::GetMaxAvailableStarLevel(int32 PlayerIncome) const
{
	if (!IsUnlocked(PlayerIncome))
	{
		return -1;
	}

	// 基础怪物没有升星机制，解锁即为 0 星
	if (!bIsUpgradedEnemy)
	{
		return 0;
	}

	int32 MaxStar = 0;
	for (int32 Star = 1; Star <= 5; ++Star)
	{
		if (PlayerIncome >= GetStarIncomeRequirement(Star))
		{
			MaxStar = Star;
		}
		else
		{
			break;
		}
	}

	return MaxStar;
}

bool UEnemyDataAsset::IsUnlocked(int32 PlayerIncome) const
{
	if (bDefaultUnlocked)
	{
		return true;
	}

	return PlayerIncome >= GetUnlockIncomeRequirement();
}
