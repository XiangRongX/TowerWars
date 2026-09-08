// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SummonOptionButton.generated.h"

class UButton;
class UImage;
class UBuildTooltip;
class UEnemyDataAsset;
class UTextBlock;
class USummonTooltip;
class ATWPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSummonOptionClicked, UEnemyDataAsset*, SelectedEnemyData);

/**
 * 
 */
UCLASS()
class TOWERWARS_API USummonOptionButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpgradeEnemy();

	FOnSummonOptionClicked OnSummonOptionClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Enemy;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Lock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Count;

	UPROPERTY(EditAnywhere, Category = "TW|UI")
	TSubclassOf<USummonTooltip> TooltipClass;

	UPROPERTY(EditAnywhere, Category = "TW|Data")
	TObjectPtr<UEnemyDataAsset> EnemyData;

private:
	bool bIsLocked = true;

	void UpdateAffordability();
	void UpdateButtonCountText();
	void HandleStockUpdated(int32 CurrentStock, int32 MaxStock, const UEnemyDataAsset* UpdatedEnemyData);

	UFUNCTION()
	void OnSummonnButtonClicked();

	UFUNCTION()
	void HandleGoldChanged(int32 NewGold);

	void BindPlayerStateEvents(ATWPlayerState* PS);
	void HandlePlayerStateReady(ATWPlayerState* PS);
};
