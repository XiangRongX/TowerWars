// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TWGameMode.h"
#include "Game/TWGameState.h"
#include "Game/TWGameInstance.h"
#include "Grid/TWGridSubsystem.h"
#include "Player/TWPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Actor/EnemySpawner.h"
#include "EngineUtils.h"

void ATWGameMode::RegisterEnemySpawner(int32 PlayerIndex, AEnemySpawner* Spawner)
{
	if (Spawner)
	{
		EnemySpawnerMap.Add(PlayerIndex, Spawner);
	}
}

void ATWGameMode::SummonEnemyToAllOthers(int32 SenderPlayerIndex, const UEnemyDataAsset* EnemyData)
{
	if (!EnemyData) return;

	ATWPlayerState* SummonerPS = GetPlayerStateByPlayerIndex(SenderPlayerIndex);

	for (const TPair<int32, AEnemySpawner*>& Pair : ActiveSpawnerMap)
	{
		const int32 TargetPlayerIndex = Pair.Key;
		AEnemySpawner* TargetSpawner = Pair.Value;

		/*UE_LOG(LogTemp, Log, TEXT("SummonEnemyToAllOthers: Sender=%d  Iterating Target=%d  Spawner=%s  IsValid=%d"),
			SenderPlayerIndex,
			TargetPlayerIndex,
			*GetNameSafe(TargetSpawner),
			IsValid(TargetSpawner) ? 1 : 0);*/

		if (TargetPlayerIndex != SenderPlayerIndex && IsValid(TargetSpawner))
		{
			TargetSpawner->AddEnemyToQueue(EnemyData, SummonerPS);
		}
	}
}

void ATWGameMode::InitializeSpawnersForPlayers(int32 ActivePlayerCount)
{
	TArray<int32> UsedSpawnerIndices;

	switch (ActivePlayerCount)
	{
	case 2:
		UsedSpawnerIndices = { 0, 1 };
		break;
	case 4:
		UsedSpawnerIndices = { 0, 1, 2, 3 };
		break;
	case 6:
	default:
		UsedSpawnerIndices = { 0, 1, 2, 3, 4, 5 };
		break;
	}

	for (int32 PlayerIdx = 0; PlayerIdx < UsedSpawnerIndices.Num(); ++PlayerIdx)
	{
		int32 TargetSpawnerIdx = UsedSpawnerIndices[PlayerIdx];

		if (AEnemySpawner** FoundSpawner = EnemySpawnerMap.Find(TargetSpawnerIdx))
		{
			AEnemySpawner* Spawner = *FoundSpawner;
			if (Spawner)
			{
				Spawner->InitializeSpawner(PlayerIdx);
				ActiveSpawnerMap.Add(PlayerIdx, Spawner);
				EnemySpawnerMap.Remove(TargetSpawnerIdx);
			}
		}
	}

	// 清理剩余没用到的 Spawner
	for (auto& Pair : EnemySpawnerMap)
	{
		if (Pair.Value)
		{
			Pair.Value->DeactivateSpawner();
		}
	}
	EnemySpawnerMap.Empty();
}

ATWPlayerState* ATWGameMode::GetPlayerStateByPlayerIndex(int32 TargetPlayerIndex) const
{
	if (!GameState) return nullptr;

	// 遍历已连接的所有 PlayerState
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ATWPlayerState* TWPS = Cast<ATWPlayerState>(PS))
		{
			// 匹配自定义的 PlayerIndex 变量
			if (TWPS->GetPlayerIndex() == TargetPlayerIndex)
			{
				return TWPS;
			}
		}
	}

	return nullptr;
}

void ATWGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UTWGridSubsystem* GridSubsystem = GetWorld()->GetSubsystem<UTWGridSubsystem>())
	{
		GridSubsystem->InitializePlayerGrid(0, FIntPoint(0, 0), FIntPoint(20, 30));
		GridSubsystem->InitializePlayerGrid(1, FIntPoint(25, 0), FIntPoint(20, 30));
	}

	for (TActorIterator<AEnemySpawner> It(GetWorld()); It; ++It)
	{
		AEnemySpawner* Spawner = *It;
		if (Spawner)
		{
			EnemySpawnerMap.Add(Spawner->SpawnerIndex, Spawner);
		}
	}
}

void ATWGameMode::InitGameState()
{
	Super::InitGameState();

	UTWGameInstance* GI = GetGameInstance<UTWGameInstance>();
	ATWGameState* GS = GetGameState<ATWGameState>();
	if (GI && GS)
	{
		GS->TargetTotalPlayers = GI->TargetTotalPlayers;
	}
}

void ATWGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ATWPlayerState* PS = NewPlayer->GetPlayerState<ATWPlayerState>();
	if (PS)
	{
		int32 AssignedIndex = NextPlayerIndex++;
		PS->SetPlayerIndex(AssignedIndex);

		UE_LOG(LogTemp, Log, TEXT("玩家 %s 已登录，分配的 PlayerIndex 为: %d"), *NewPlayer->GetName(), AssignedIndex);
	}

	ATWGameState* GS = GetGameState<ATWGameState>();
	if (GS)
	{
		// 当当前在线玩家数达到了 GameState 设置的目标人数
		if (GetNumPlayers() == GS->TargetTotalPlayers)
		{
			UE_LOG(LogTemp, Log, TEXT("玩家已满 (%d/%d)，开始分配跑道与 Spawner！"), GetNumPlayers(), GS->TargetTotalPlayers);
			InitializeSpawnersForPlayers(GS->TargetTotalPlayers);
		}
	}
}

void ATWGameMode::HandlePlayerEliminated(ATWPlayerState* EliminatedPlayer)
{
	if (!EliminatedPlayer) return;

	ATWGameState* GS = GetGameState<ATWGameState>();
	if (!GS || GS->IsGameOver()) return;

	UE_LOG(LogTemp, Log, TEXT("玩家 %d 被淘汰"), EliminatedPlayer->GetPlayerIndex());

	CheckLastPlayerStanding();
}

void ATWGameMode::CheckLastPlayerStanding()
{
	ATWGameState* GS = GetGameState<ATWGameState>();
	if (!GS || GS->IsGameOver()) return;

	TArray<int32> AlivePlayerIndices;
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ATWPlayerState* TWPS = Cast<ATWPlayerState>(PS))
		{
			if (TWPS->GetPlayerHealth() > 0)
			{
				AlivePlayerIndices.Add(TWPS->GetPlayerIndex());
			}
		}
	}

	// 只剩最后一个存活玩家
	if (AlivePlayerIndices.Num() == 1)
	{
		UE_LOG(LogTemp, Log, TEXT("只剩最后一名玩家存活，PlayerIndex=%d 获胜"), AlivePlayerIndices[0]);

		GS->EndGameWithWinners(AlivePlayerIndices);
	}
}

