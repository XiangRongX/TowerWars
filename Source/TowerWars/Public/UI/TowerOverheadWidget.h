// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerOverheadWidget.generated.h"

class UTextBlock;
class ATWTowerBase;
class ATWPlayerState;

/**
 * 
 */
UCLASS()
class TOWERWARS_API UTowerOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitTower(ATWTowerBase* InTower);
	void Refresh();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Upgrade;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_TowerName;

private:
	UPROPERTY()
	TWeakObjectPtr<ATWTowerBase> Tower;

	UPROPERTY()
	TWeakObjectPtr<ATWPlayerState> PlayerState;

	UFUNCTION()
	void OnGoldChanged(int32 NewGold);

	void UpdateUpgradeVisibility();
};
