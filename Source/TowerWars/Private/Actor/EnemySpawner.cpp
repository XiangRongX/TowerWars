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
	if (!HasAuthority() || SpawnQueue.Num() == 0 || !TargetPathActor) return;

	FEnemySpawnRequest Request = SpawnQueue[0];
	SpawnQueue.RemoveAt(0);

	USplineComponent* SplineComp = TargetPathActor->GetSplineComponent();
	if (!SplineComp) return;

	FVector SpawnLoc = SplineComp->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
	FRotator SpawnRot = SplineComp->GetRotationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);

	ATWEnemyBase* NewEnemy = GetWorld()->SpawnActorDeferred<ATWEnemyBase>(
		Request.EnemyData->EnemyClass,
		FTransform(SpawnRot, SpawnLoc),
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (NewEnemy)
	{
		NewEnemy->InitEnemy(Request.EnemyData, SplineComp, OwnerPlayerIndex);
		NewEnemy->FinishSpawning(FTransform(SpawnRot, SpawnLoc));
		NewEnemy->SetSummonerPlayerState(Request.SummonerPS);
	}
}


