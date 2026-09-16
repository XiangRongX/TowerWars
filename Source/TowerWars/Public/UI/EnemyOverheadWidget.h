// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyOverheadWidget.generated.h"

class UTextBlock;
class ATWEnemyBase;
class UHorizontalBox;

/**
 * 
 */
UCLASS()
class TOWERWARS_API UEnemyOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitEnemy(ATWEnemyBase* InEnemy);

	void ShowAliveState();
	void ShowDeathReward(int32 GoldReward, int32 IncomeReward);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	FLinearColor GetPlayerColor(int32 PlayerIndex) const;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Player;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Health;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Gold;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Income;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> Box_Info;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> Box_DeathReward;

private:
	TWeakObjectPtr<ATWEnemyBase> Enemy;

};
