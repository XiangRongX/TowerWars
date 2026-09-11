// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TWPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ATWHUD;
class UTowerDataAsset;
class UEnemyDataAsset;
class ATWPlayerState;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateReady, ATWPlayerState* /*PlayerState*/);

/**
 * 
 */
UCLASS()
class TOWERWARS_API ATWPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitPlayerState() override;

	FOnPlayerStateReady OnPlayerStateReady;
	
	UFUNCTION(BlueprintCallable, Category = "TW|Build")
	void RequestBuildTower(UTowerDataAsset* TowerData);

	UFUNCTION(BlueprintCallable, Category = "TW|Build")
	void RequestUpgradeTower(UTowerDataAsset* TargetTowerData);

	UFUNCTION(BlueprintCallable, Category = "TW|Build")
	void RequestSellTower();

	UFUNCTION(BlueprintCallable, Category = "TW|Summon")
	void RequestSummonEnemy(UEnemyDataAsset* EnemyData);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category = "TW|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "TW|Input")
	TObjectPtr<UInputAction> ClickAction;

	UPROPERTY(EditAnywhere, Category = "TW|Input")
	TObjectPtr<UInputAction> SummonAction;

	void OnLeftClick();
	void OnSummonClick();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestBuildTower(FIntPoint GridCoord, UTowerDataAsset* TowerData);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestUpgradeTower(FIntPoint GridCoord, UTowerDataAsset* TargetTowerData);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSellTower(FIntPoint GridCoord);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSummonEnemy(UEnemyDataAsset* EnemyData);

private:
	TWeakObjectPtr<ATWHUD> HUD;

	FIntPoint SelectedGridCoord;

	void NotifyPlayerStateReady();
};
