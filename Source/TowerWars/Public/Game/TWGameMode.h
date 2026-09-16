// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TWGameMode.generated.h"

class AEnemySpawner;
class UEnemyDataAsset;
class ATWPlayerState;

/**
 * 
 */
UCLASS()
class TOWERWARS_API ATWGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	void RegisterEnemySpawner(int32 PlayerIndex, AEnemySpawner* Spawner);
	void SummonEnemyToAllOthers(int32 SenderPlayerIndex, const UEnemyDataAsset* EnemyData);
	void InitializeSpawnersForPlayers(int32 ActivePlayerCount);
	ATWPlayerState* GetPlayerStateByPlayerIndex(int32 TargetPlayerIndex) const;
	void HandlePlayerEliminated(ATWPlayerState* EliminatedPlayer);
	
protected:
	virtual void BeginPlay() override;
	virtual void InitGameState() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	int32 NextPlayerIndex = 0;

	TMap<int32, AEnemySpawner*> EnemySpawnerMap;
	TMap<int32, AEnemySpawner*> ActiveSpawnerMap;

	void CheckLastPlayerStanding();
};
