// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyDataAsset.generated.h"

class ATWEnemyBase;

/**
 * 
 */
UCLASS()
class TOWERWARS_API UEnemyDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FText EnemyName;

	UPROPERTY(EditDefaultsOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly)
	FText Special;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATWEnemyBase> EnemyClass;

	UPROPERTY(EditDefaultsOnly)
	int32 Cost;

	UPROPERTY(EditDefaultsOnly)
	int32 Income;

	UPROPERTY(EditDefaultsOnly)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	float Speed;

	UPROPERTY(EditDefaultsOnly)
	bool bIsUpgradedEnemy;

	UPROPERTY(EditDefaultsOnly)
	bool bDefaultUnlocked;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UEnemyDataAsset> NextUpgrade;

	/** 计算指定星级 (0~5) 下的动态 Cost = 0星Cost * 1.7^StarLevel */
	int32 GetCostForStar(int32 StarLevel) const;

	/** 计算指定星级 (0~5) 下的动态 Income = 0星Income * 1.2^StarLevel */
	int32 GetIncomeForStar(int32 StarLevel) const;

	/** 获取解锁该怪物（0星）所需的最低玩家收入 */
	int32 GetUnlockIncomeRequirement() const;

	/** 获取升到目标星级 (1~5) 所需的玩家收入 */
	int32 GetStarIncomeRequirement(int32 TargetStar) const;

	/** 根据玩家当前收入，获取允许升到的最高星级（未解锁返回 -1，基础怪物返回 0） */
	int32 GetMaxAvailableStarLevel(int32 PlayerIncome) const;

	/** 检查玩家当前收入是否已解锁该怪物（0星） */
	bool IsUnlocked(int32 PlayerIncome) const;
};
