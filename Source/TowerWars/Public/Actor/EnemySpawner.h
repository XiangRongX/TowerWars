// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class APathActor;
class UEnemyDataAsset;

USTRUCT(BlueprintType)
struct FEnemySpawnRequest
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<const UEnemyDataAsset> EnemyData = nullptr;

	UPROPERTY()
	TObjectPtr<ATWPlayerState> SummonerPS = nullptr;
};

USTRUCT(BlueprintType)
struct FEnemyRouteType
{
	GENERATED_BODY()

	/** 路线类型名称 */
	UPROPERTY(EditInstanceOnly, Category = "Route")
	FName RouteTypeName = NAME_None;

	/** 该路线类型被随机选中的权重 */
	UPROPERTY(EditInstanceOnly, Category = "Route")
	float Weight = 1.0f;

	/** 该路线类型下的完整路线 */
	UPROPERTY(EditInstanceOnly, Category = "Route")
	TArray<TObjectPtr<APathActor>> Paths;
};

UCLASS()
class TOWERWARS_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();

	void InitializeSpawner(int32 InPlayerIndex);
	void DeactivateSpawner();

	UPROPERTY(EditInstanceOnly, Category = "TW|Spawn")
	int32 SpawnerIndex = 0;

	UPROPERTY(EditInstanceOnly, Category = "TW|Spawn")
	TArray<FEnemyRouteType> RouteTypes;

	UFUNCTION(BlueprintCallable)
	void AddEnemyToQueue(const UEnemyDataAsset* EnemyData, ATWPlayerState* PS);

	int32 GetOwnerPlayerIndex() const { return OwnerPlayerIndex; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "TW|Spawn")
	float SpawnInterval = 0.5f;

private:
	TArray<FEnemySpawnRequest> SpawnQueue;
	FTimerHandle SpawnTimerHandle;
	int32 OwnerPlayerIndex = INDEX_NONE;

	void ProcessSpawnQueue();
	const FEnemyRouteType* SelectRandomRouteType() const;
	APathActor* SelectRandomPath(const FEnemyRouteType& RouteType) const;
};
