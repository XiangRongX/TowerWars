// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildOptionButton.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UBuildTooltip;
class UTowerDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildOptionClicked, UTowerDataAsset*, SelectedTowerData);

/**
 * 
 */
UCLASS()
class TOWERWARS_API UBuildOptionButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnBuildOptionClicked OnBuildOptionClicked;
	FOnBuildOptionClicked OnUpgradeOptionClicked;

	void SetLocked(bool bLocked);
	void InitUpgradeOption(UTowerDataAsset* Data);

	FORCEINLINE bool GetLocked() const { return bIsLocked; }
	FORCEINLINE void SetBuild(bool bBuild) { bIsBuild = bBuild; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Tower;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Lock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Level;

	UPROPERTY(EditAnywhere, Category = "TW|UI")
	TSubclassOf<UBuildTooltip> TooltipClass;

	UPROPERTY(EditAnywhere, Category = "TW|Data")
	TObjectPtr<UTowerDataAsset> TowerData;

private:
	bool bIsLocked = true;
	bool bIsBuild = true;

	UFUNCTION()
	void OnTowerButtonClicked();

	UFUNCTION()
	void HandleGoldChanged(int32 NewGold);

	void UpdateAffordability(int32 CurrentGold);
};
