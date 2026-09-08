// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TWGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, int32 /*SecondsRemaining*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateAdded, APlayerState* /*NewPlayerState*/);

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

	// UI 绑定的委托
	FOnTimerUpdated OnMatchTimerUpdated;
	FOnTimerUpdated OnIncomeTimerUpdated;
	FOnPlayerStateAdded OnPlayerStateAdded;

	FORCEINLINE int32 GetMatchTimeRemaining() const { return MatchTimeRemaining; }
	FORCEINLINE int32 GetIncomeTimeRemaining() const { return IncomeTimeRemaining; }

	UPROPERTY(Replicated)
	int32 TargetTotalPlayers = 4;

protected:
	virtual void BeginPlay() override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	UPROPERTY(ReplicatedUsing = OnRep_MatchTimeRemaining)
	int32 MatchTimeRemaining = 3600; 

	UPROPERTY(ReplicatedUsing = OnRep_IncomeTimeRemaining)
	int32 IncomeTimeRemaining = 10; 

private:
	UFUNCTION()
	void OnRep_MatchTimeRemaining();

	UFUNCTION()
	void OnRep_IncomeTimeRemaining();

	FTimerHandle SecondTickTimerHandle;

	void OnSecondTick();
	void DistributePeriodicIncome();
};
