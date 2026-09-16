// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TWGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, int32 /*SecondsRemaining*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateAdded, APlayerState* /*NewPlayerState*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyHealthMultiplierChanged, float /*NewMultiplier*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameEnded, const TArray<int32>& /*WinnerPlayerIndices*/);

/**
 * 
 */
UCLASS()
class TOWERWARS_API ATWGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ATWGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EndGameWithWinners(const TArray<int32>& InWinnerPlayerIndices);

	// UI 绑定的委托
	FOnTimerUpdated OnMatchTimerUpdated;
	FOnTimerUpdated OnIncomeTimerUpdated;
	FOnPlayerStateAdded OnPlayerStateAdded;
	// 全局怪物血量倍率变化
	FOnEnemyHealthMultiplierChanged OnEnemyHealthMultiplierChanged;
	// 游戏结束
	FOnGameEnded OnGameEnded;

	FORCEINLINE int32 GetMatchTimeRemaining() const { return MatchTimeRemaining; }
	FORCEINLINE int32 GetIncomeTimeRemaining() const { return IncomeTimeRemaining; }
	FORCEINLINE float GetEnemyHealthMultiplier() const { return EnemyHealthMultiplier; }
	FORCEINLINE bool IsGameOver() const { return bGameOver; }
	FORCEINLINE const TArray<int32>& GetWinnerPlayerIndices() const { return WinnerPlayerIndices; }

	UPROPERTY(Replicated)
	int32 TargetTotalPlayers = 4;

protected:
	virtual void BeginPlay() override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	UPROPERTY(ReplicatedUsing = OnRep_MatchTimeRemaining)
	int32 MatchTimeRemaining = 3600; 

	UPROPERTY(ReplicatedUsing = OnRep_IncomeTimeRemaining)
	int32 IncomeTimeRemaining = 10; 

	UPROPERTY(ReplicatedUsing = OnRep_EnemyHealthMultiplier)
	float EnemyHealthMultiplier = 1.0f;

	UPROPERTY(ReplicatedUsing = OnRep_GameOver, BlueprintReadOnly, Category = "TW|Game")
	bool bGameOver = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "TW|Game")
	TArray<int32> WinnerPlayerIndices;

private:
	UFUNCTION()
	void OnRep_MatchTimeRemaining();

	UFUNCTION()
	void OnRep_IncomeTimeRemaining();

	UFUNCTION()
	void OnRep_EnemyHealthMultiplier();

	UFUNCTION()
	void OnRep_GameOver();

	FTimerHandle SecondTickTimerHandle;

	void OnSecondTick();
	void DistributePeriodicIncome();

	// 距离上一次怪物血量倍率提升经过的时间
	int32 EnemyHealthScaleElapsedSeconds = 0;
	static constexpr int32 EnemyHealthScaleInterval = 150;
	static constexpr float EnemyHealthScaleFactor = 1.13f;

	void DetermineWinnersByHealth();
	void BroadcastGameEnded();
};
