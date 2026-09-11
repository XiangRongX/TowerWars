// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TWPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Data/EnemyDataAsset.h"

ATWPlayerState::ATWPlayerState()
{
	NetUpdateFrequency = 30.0f;
}

void ATWPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATWPlayerState, PlayerHealth);
	DOREPLIFETIME(ATWPlayerState, PlayerIndex);

	DOREPLIFETIME_CONDITION(ATWPlayerState, Gold, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATWPlayerState, Income, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATWPlayerState, MaxStock, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATWPlayerState, CurrentStock, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATWPlayerState, SummonedEnemyList, COND_OwnerOnly);
}

void ATWPlayerState::AddGold(int32 Amount)
{
	if (!HasAuthority()) return;

	Gold += Amount;
	OnGoldChanged.Broadcast(Gold);
}

void ATWPlayerState::AddHealth(int32 Amount)
{
	if (!HasAuthority()) return;

	PlayerHealth += Amount;
	OnHealthChanged.Broadcast(PlayerIndex, PlayerHealth);
}

void ATWPlayerState::ReduceHealth(int32 Amount)
{
	if (!HasAuthority() || Amount <= 0) return;

	PlayerHealth = FMath::Max(0, PlayerHealth - Amount);
	OnHealthChanged.Broadcast(PlayerIndex, PlayerHealth);

	if(PlayerHealth <= 0)
	{
		// 玩家血量为 0，触发游戏结束逻辑
		// 可以在这里广播一个事件，或者调用游戏模式的相关方法
	}
}

void ATWPlayerState::AddIncome(int32 Amount)
{
	if (!HasAuthority()) return;

	Income += Amount;
	OnIncomeChanged.Broadcast(Income);
}

void ATWPlayerState::ApplyPeriodicIncome()
{
	if (!HasAuthority()) return;

	AddGold(Income);
}

void ATWPlayerState::SetPlayerIndex(int32 Index)
{
	if (!HasAuthority()) return;

	PlayerIndex = Index;
	OnPlayerIndexChanged.Broadcast(PlayerIndex);
}

bool ATWPlayerState::TryConsumeStock(UEnemyDataAsset* EnemyData, int32 Amount)
{
	if (!HasAuthority() || !EnemyData || Amount <= 0) return false;

	// 校验当前可用 Stock 是否充足
	if (CurrentStock >= Amount)
	{
		// 1. 扣除共享 Stock
		CurrentStock -= Amount;

		// 2. 增加特定怪物的累计召唤数（只增不减）
		FEnemySummonInfo* FoundInfo = SummonedEnemyList.FindByPredicate([EnemyData](const FEnemySummonInfo& Info) {
			return Info.EnemyData == EnemyData;
			});

		if (FoundInfo)
		{
			FoundInfo->TotalSummonedCount += Amount;
		}
		else
		{
			FEnemySummonInfo NewInfo;
			NewInfo.EnemyData = EnemyData;
			NewInfo.TotalSummonedCount = Amount;
			SummonedEnemyList.Add(NewInfo);
		}

		BroadcastStockUpdate(EnemyData);
		return true;
	}

	return false;
}

int32 ATWPlayerState::GetSummonedCountForEnemy(const UEnemyDataAsset* EnemyData) const
{
	if (!EnemyData) return 0;

	const FEnemySummonInfo* FoundInfo = SummonedEnemyList.FindByPredicate([EnemyData](const FEnemySummonInfo& Info) {
		return Info.EnemyData == EnemyData;
		});

	return FoundInfo ? FoundInfo->TotalSummonedCount : 0;
}

int32 ATWPlayerState::GetEnemyStarLevel(const UEnemyDataAsset* EnemyData) const
{
	if (!EnemyData) return 0;

	const FEnemySummonInfo* FoundInfo = SummonedEnemyList.FindByPredicate([EnemyData](const FEnemySummonInfo& Info) {
		return Info.EnemyData == EnemyData;
		});

	return FoundInfo ? FoundInfo->StarLevel : 0;
}

void ATWPlayerState::Server_UpgradeEnemyStar_Implementation(UEnemyDataAsset* EnemyData)
{
	if (!HasAuthority() || !EnemyData) return;

	// 1. 如果传入的是基础怪物，且存在 NextUpgrade，代表请求从【基础怪物】升级为【升级怪物】
	UEnemyDataAsset* TargetEnemy = EnemyData;
	if (!EnemyData->bIsUpgradedEnemy)
	{
		if (!EnemyData->NextUpgrade) return;
		TargetEnemy = EnemyData->NextUpgrade;
	}

	const int32 CurrentStar = GetEnemyStarLevel(TargetEnemy);
	const int32 TargetStar = (EnemyData->bIsUpgradedEnemy) ? (CurrentStar + 1) : 0;

	if (TargetStar > 5) return;

	// 2. 校验玩家 Income 是否达到升星/解锁要求
	const int32 RequiredIncome = (TargetStar == 0)
		? TargetEnemy->GetUnlockIncomeRequirement()
		: TargetEnemy->GetStarIncomeRequirement(TargetStar);

	if (Income < RequiredIncome) return;

	// 3. 更新 SummonedEnemyList（通过 Replicated 自动同步）
	FEnemySummonInfo* FoundInfo = SummonedEnemyList.FindByPredicate([TargetEnemy](const FEnemySummonInfo& Info) {
		return Info.EnemyData == TargetEnemy;
		});

	if (FoundInfo)
	{
		FoundInfo->StarLevel = TargetStar;
	}
	else
	{
		FEnemySummonInfo NewInfo;
		NewInfo.EnemyData = TargetEnemy;
		NewInfo.StarLevel = TargetStar;
		SummonedEnemyList.Add(NewInfo);
	}

	// 4. 广播星级提升
	OnEnemyStarUpgraded.Broadcast(TargetEnemy, TargetStar);
	BroadcastStockUpdate(TargetEnemy);
}

void ATWPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// 启动定时器，每秒调用一次 OnStockTick
		GetWorldTimerManager().SetTimer(StockTimerHandle, this, &ATWPlayerState::OnStockTick, 1.0f, true);
	}
}

void ATWPlayerState::OnRep_Gold(int32 OldGold)
{
	OnGoldChanged.Broadcast(Gold);
}

void ATWPlayerState::OnRep_Income(int32 OldIncome)
{
	OnIncomeChanged.Broadcast(Income);
}

void ATWPlayerState::OnRep_PlayerHealth(int32 OldHealth)
{
	OnHealthChanged.Broadcast(PlayerIndex, PlayerHealth);
}

void ATWPlayerState::OnRep_PlayerIndex()
{
	OnPlayerIndexChanged.Broadcast(PlayerIndex);
}

void ATWPlayerState::OnRep_StockChanged()
{
	BroadcastStockUpdate();
}

void ATWPlayerState::OnStockTick()
{
	ElapsedSeconds++;
	bool bChanged = false;

	// 1. 每 10 秒增加 1 点 MaxStock（上限 30）
	if (ElapsedSeconds % 10 == 0 && MaxStock < 30)
	{
		MaxStock = FMath::Min(30, MaxStock + 1);
		bChanged = true;
	}

	// 2. 每 1 秒恢复 1 点 Stock（不能超过 MaxStock）
	if (CurrentStock < MaxStock)
	{
		CurrentStock = FMath::Min(MaxStock, CurrentStock + 1);
		bChanged = true;
	}

	if (bChanged)
	{
		BroadcastStockUpdate();
	}
}

void ATWPlayerState::BroadcastStockUpdate(const UEnemyDataAsset* EnemyData)
{
	OnPlayerStockUpdated.Broadcast(CurrentStock, MaxStock, EnemyData);
}
