// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SellButton.generated.h"

class UButton;
class USellTooltip;
class UTowerDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSellClicked, UTowerDataAsset*, SelectedTowerData);

/**
 * 
 */
UCLASS()
class TOWERWARS_API USellButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnSellClicked OnSellClicked;

	void InitSellButton(UTowerDataAsset* Data);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Sell;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TW|UI")
	TSubclassOf<USellTooltip> TooltipClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TW|Data")
	TObjectPtr<UTowerDataAsset> TowerData;

private:
	UFUNCTION()
	void OnSellButtonClicked();

};
