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

void ATWGameState::OnSecondTick()
{
	if (!HasAuthority()) return;

	if (MatchTimeRemaining > 0)
	{
		MatchTimeRemaining--;
		OnRep_MatchTimeRemaining(); 
	}

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
}

void ATWGameState::DistributePeriodicIncome()
{
	for (APlayerState* PS : PlayerArray)
	{
		if (ATWPlayerState* TWPS = Cast<ATWPlayerState>(PS))
		{
			TWPS->ApplyPeriodicIncome();
		}
	}
}
