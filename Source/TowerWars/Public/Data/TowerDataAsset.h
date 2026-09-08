// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Type/TWType.h"
#include "TowerDataAsset.generated.h"

class ATWTowerBase;

/**
 * 
 */
UCLASS()
class TOWERWARS_API UTowerDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FText TowerName;

	UPROPERTY(EditDefaultsOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly)
	FText Special;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATWTowerBase> TowerClass;

	UPROPERTY(EditDefaultsOnly)
	int32 Cost;

	UPROPERTY(EditDefaultsOnly)
	int32 Sell;

	UPROPERTY(EditDefaultsOnly)
	int32 Level;

	UPROPERTY(EditDefaultsOnly)
	bool bDefaultUnlocked = true;

	UPROPERTY(EditDefaultsOnly)
	float Damage;

	UPROPERTY(EditDefaultsOnly)
	float AttackRange;

	UPROPERTY(EditDefaultsOnly)
	float AttackInterval;

	UPROPERTY(EditDefaultsOnly)
	float SplashRadius;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UTowerDataAsset>> NextUpgrades;

	UPROPERTY(EditDefaultsOnly)
	ETowerDamageType DamageType;

	UPROPERTY(EditDefaultsOnly)
	ETowerTargetStrategy TargetStrategy;
};
