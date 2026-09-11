// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TWPlayerState.generated.h"

class UEnemyDataAsset;

USTRUCT(BlueprintType)
struct FEnemySummonInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UEnemyDataAsset> EnemyData = nullptr;

	UPROPERTY()
	int32 TotalSummonedCount = 0;

	UPROPERTY()
	int32 StarLevel = 0;

	bool operator==(const FEnemySummonInfo& Other) const
	{
		return EnemyData == Other.EnemyData;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDataChanged, int32, NewData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, int32, PlayerIndex, int32, NewHealth);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPlayerStockUpdated, int32 /*CurrentStock*/, int32 /*MaxStock*/, const UEnemyDataAsset* /*UpdatedEnemyData*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyStarUpgraded, UEnemyDataAsset*, EnemyData, int32, NewStarLevel);
/**
 * 
 */
UCLASS()
class TOWERWARS_API ATWPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ATWPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnDataChanged OnGoldChanged;
	FOnDataChanged OnIncomeChanged;
	FOnDataChanged OnPlayerIndexChanged;
	FOnHealthChanged OnHealthChanged;
	FOnPlayerStockUpdated OnPlayerStockUpdated;
	FOnEnemyStarUpgraded OnEnemyStarUpgraded;

	void AddGold(int32 Amount);
	void AddHealth(int32 Amount);
	void ReduceHealth(int32 Amount);
	UFUNCTION(BlueprintCallable)
	void AddIncome(int32 Amount);
	void ApplyPeriodicIncome();
	void SetPlayerIndex(int32 Index);
	bool TryConsumeStock(UEnemyDataAsset* EnemyData, int32 Amount = 1);
	int32 GetSummonedCountForEnemy(const UEnemyDataAsset* EnemyData) const;
	int32 GetEnemyStarLevel(const UEnemyDataAsset* EnemyData) const;

	// 请求升级怪物星级（客户端发起）
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "TW|Enemy")
	void Server_UpgradeEnemyStar(UEnemyDataAsset* EnemyData);

	FORCEINLINE int32 GetPlayerIndex() const { return PlayerIndex; }
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetIncome() const { return Income; }
	FORCEINLINE int32 GetPlayerHealth() const { return PlayerHealth; }
	FORCEINLINE int32 GetCurrentStock() const { return CurrentStock; }
	FORCEINLINE int32 GetMaxStock() const { return MaxStock; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerIndex)
	int32 PlayerIndex = -1;

	UPROPERTY(ReplicatedUsing = OnRep_Gold)
	int32 Gold = 75;

	UPROPERTY(ReplicatedUsing = OnRep_Income)
	int32 Income = 5;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerHealth)
	int32 PlayerHealth = 20;

	UPROPERTY(ReplicatedUsing = OnRep_StockChanged, BlueprintReadOnly, Category = "TW|Stock")
	int32 MaxStock = 0;

	UPROPERTY(ReplicatedUsing = OnRep_StockChanged, BlueprintReadOnly, Category = "TW|Stock")
	int32 CurrentStock = 0;

	// 每种怪物的累计召唤数列表（网络同步）
	UPROPERTY(ReplicatedUsing = OnRep_StockChanged, BlueprintReadOnly, Category = "TW|Stock")
	TArray<FEnemySummonInfo> SummonedEnemyList;

private:
	UFUNCTION()
	void OnRep_Gold(int32 OldGold);

	UFUNCTION()
	void OnRep_Income(int32 OldIncome);

	UFUNCTION()
	void OnRep_PlayerHealth(int32 OldHealth);

	UFUNCTION()
	void OnRep_PlayerIndex();

	UFUNCTION()
	void OnRep_StockChanged();

	FTimerHandle StockTimerHandle;
	int32 ElapsedSeconds = 0;
	void OnStockTick();
	void BroadcastStockUpdate(const UEnemyDataAsset* EnemyData = nullptr);

	TMap<TObjectPtr<UEnemyDataAsset>, int32> EnemyStarLevels;
};
