// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeMenu.generated.h"

class UTowerDataAsset;
class UBuildOptionButton;
class USellButton;

/**
 * 
 */
UCLASS()
class TOWERWARS_API UUpgradeMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitUpgradeMenu(UTowerDataAsset* TowerData);

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBuildOptionButton> WBP_BuildOptionButton1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBuildOptionButton> WBP_BuildOptionButton2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USellButton> WBP_SellButton;

private:
	UFUNCTION()
	void HandleTowerOptionSelected(UTowerDataAsset* SelectedTowerData);

	UFUNCTION()
	void HandleSellSelected(UTowerDataAsset* SelectedTowerData);
};
