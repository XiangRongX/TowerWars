// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TWGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/TWPlayerState.h"

ATWGameState::ATWGameState()
{
	bReplicates = true;
}

void ATWGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATWGameState, TargetTotalPlayers);
	DOREPLIFETIME(ATWGameState, MatchTimeRemaining);
	DOREPLIFETIME(ATWGameState, IncomeTimeRemaining);
	DOREPLIFETIME(ATWGameState, EnemyHealthMultiplier);
	DOREPLIFETIME(ATWGameState, bGameOver);
	DOREPLIFETIME(ATWGameState, WinnerPlayerIndices);
}

void ATWGameState::EndGameWithWinners(const TArray<int32>& InWinnerPlayerIndices)
{
	if (!HasAuthority() || bGameOver) return;

	WinnerPlayerIndices = InWinnerPlayerIndices;
	bGameOver = true;

	GetWorldTimerManager().ClearTimer(SecondTickTimerHandle);

	BroadcastGameEnded();
}

void ATWGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SecondTickTimerHandle, this, &ATWGameState::OnSecondTick, 1.0f,	true);
	}
}

void ATWGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (PlayerState)
	{
		OnPlayerStateAdded.Broadcast(PlayerState);
	}
}

void ATWGameState::OnRep_MatchTimeRemaining()
{
	OnMatchTimerUpdated.Broadcast(MatchTimeRemaining);
}

void ATWGameState::OnRep_IncomeTimeRemaining()
{
	OnIncomeTimerUpdated.Broadcast(IncomeTimeRemaining);
}

void ATWGameState::OnRep_EnemyHealthMultiplier()
{
	OnEnemyHealthMultiplierChanged.Broadcast(EnemyHealthMultiplier);
}

void ATWGameState::OnRep_GameOver()
{
	if (bGameOver)
	{
		OnGameEnded.Broadcast(WinnerPlayerIndices);
	}
}

void ATWGameState::OnSecondTick()
{
	if (!HasAuthority() || bGameOver) return;

	// 比赛时间
	if (MatchTimeRemaining > 0)
	{
		MatchTimeRemaining--;
		OnRep_MatchTimeRemaining(); 
	}

	// 收入
	IncomeTimeRemaining--;
	if (IncomeTimeRemaining <= 0)
	{
		DistributePeriodicIncome();
		IncomeTimeRemaining = 10;
	}
	OnRep_IncomeTimeRemaining();

	// 每秒累计一次怪物血量倍率计时
	EnemyHealthScaleElapsedSeconds++;
	if (EnemyHealthScaleElapsedSeconds >= EnemyHealthScaleInterval)
	{
		EnemyHealthScaleElapsedSeconds = 0;

		// 每 150 秒提升至当前倍率的 1.13 倍
		EnemyHealthMultiplier *= EnemyHealthScaleFactor;

		// 服务端本地立即通知
		OnRep_EnemyHealthMultiplier();
	}

	// 比赛结束
	if (MatchTimeRemaining <= 0)
	{
		DetermineWinnersByHealth();
	}
}

void ATWGameState::DistributePeriodicIncome()
{
	if (bGameOver) return;

	for (APlayerState* PS : PlayerArray)
	{
		if (ATWPlayerState* TWPS = Cast<ATWPlayerState>(PS))
		{
			if (TWPS->GetPlayerHealth() > 0)
			{
				TWPS->ApplyPeriodicIncome();
			}
		}
	}
}

void ATWGameState::DetermineWinnersByHealth()
{
	if (!HasAuthority() || bGameOver) return;

	// 找到最高血量
	int32 HighestHealth = -1;
	for (APlayerState* PS : PlayerArray)
	{
		if (ATWPlayerState* TWPS = Cast<ATWPlayerState>(PS))
		{
			const int32 Health = TWPS->GetPlayerHealth();

			// 血量为 0 的玩家已经淘汰，不参与胜负
			if (Health > 0)
			{
				HighestHealth = FMath::Max(HighestHealth, Health);
			}
		}
	}

	// 收集所有最高血量玩家
	WinnerPlayerIndices.Reset();
	if (HighestHealth >= 0)
	{
		for (APlayerState* PS : PlayerArray)
		{
			if (ATWPlayerState* TWPS = Cast<ATWPlayerState>(PS))
			{
				if (TWPS->GetPlayerHealth() > 0 &&
					TWPS->GetPlayerHealth() == HighestHealth)
				{
					WinnerPlayerIndices.Add(TWPS->GetPlayerIndex());
				}
			}
		}
	}

	bGameOver = true;

	GetWorldTimerManager().ClearTimer(SecondTickTimerHandle);

	BroadcastGameEnded();
}

void ATWGameState::BroadcastGameEnded()
{
	OnGameEnded.Broadcast(WinnerPlayerIndices);
}
