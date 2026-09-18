// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/EnemySpawner.h"
#include "Actor/PathActor.h"
#include "Data/EnemyDataAsset.h"
#include "Components/SplineComponent.h"
#include "Enemy/TWEnemyBase.h"
#include "Game/TWGameMode.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = false;
}

void AEnemySpawner::InitializeSpawner(int32 InPlayerIndex)
{
	OwnerPlayerIndex = InPlayerIndex;

	// 启动刷怪定时器
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::ProcessSpawnQueue, SpawnInterval, true);
	}
}

void AEnemySpawner::DeactivateSpawner()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	Destroy();
}

void AEnemySpawner::AddEnemyToQueue(const UEnemyDataAsset* EnemyData, ATWPlayerState* SummonerPS)
{
	if(!HasAuthority() || !EnemyData) return;

	FEnemySpawnRequest NewRequest;
	NewRequest.EnemyData = EnemyData;
	NewRequest.SummonerPS = SummonerPS;

	SpawnQueue.Add(NewRequest);
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

}

void AEnemySpawner::ProcessSpawnQueue()
{
	if (!HasAuthority() || SpawnQueue.Num() == 0) return;

	// 第一层：选择路线类型
	const FEnemyRouteType* SelectedRouteType = SelectRandomRouteType();
	if (!SelectedRouteType) return;

	// 第二层：选择该路线类型下的具体完整 Path
	APathActor* SelectedPath = SelectRandomPath(*SelectedRouteType);
	if (!SelectedPath) return;

	USplineComponent* SplineComp = SelectedPath->GetSplineComponent();
	if (!SplineComp) return;

	FEnemySpawnRequest Request = SpawnQueue[0];
	SpawnQueue.RemoveAt(0);

	FVector SpawnLoc = SplineComp->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
	FRotator SpawnRot = SplineComp->GetRotationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
	ATWEnemyBase* NewEnemy = GetWorld()->SpawnActorDeferred<ATWEnemyBase>(Request.EnemyData->EnemyClass, FTransform(SpawnRot, SpawnLoc), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (NewEnemy)
	{
		NewEnemy->InitEnemy(Request.EnemyData, SplineComp, OwnerPlayerIndex);
		NewEnemy->FinishSpawning(FTransform(SpawnRot, SpawnLoc));
		NewEnemy->SetSummonerPlayerState(Request.SummonerPS);
	}
}

const FEnemyRouteType* AEnemySpawner::SelectRandomRouteType() const
{
	TArray<const FEnemyRouteType*> ValidRouteTypes;

	for (const FEnemyRouteType& RouteType : RouteTypes)
	{
		if (RouteType.Weight <= 0.0f) continue;

		bool bHasValidPath = false;
		for (APathActor* Path : RouteType.Paths)
		{
			if (IsValid(Path) && Path->GetSplineComponent())
			{
				bHasValidPath = true;
				break;
			}
		}

		if (bHasValidPath)
		{
			ValidRouteTypes.Add(&RouteType);
		}
	}

	if (ValidRouteTypes.Num() == 0) return nullptr;

	float TotalWeight = 0.0f;
	for (const FEnemyRouteType* RouteType : ValidRouteTypes)
	{
		TotalWeight += RouteType->Weight;
	}

	float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
	for (const FEnemyRouteType* RouteType : ValidRouteTypes)
	{
		RandomValue -= RouteType->Weight;

		if (RandomValue <= 0.0f)
		{
			return RouteType;
		}
	}

	return ValidRouteTypes.Last();
}

APathActor* AEnemySpawner::SelectRandomPath(const FEnemyRouteType& RouteType) const
{
	TArray<APathActor*> ValidPaths;

	for (APathActor* Path : RouteType.Paths)
	{
		if (!IsValid(Path)) continue;
		if (!Path->GetSplineComponent()) continue;

		ValidPaths.Add(Path);
	}

	if (ValidPaths.Num() == 0)
	{
		return nullptr;
	}

	const int32 RandomIndex = FMath::RandRange(0, ValidPaths.Num() - 1);

	return ValidPaths[RandomIndex];
}
