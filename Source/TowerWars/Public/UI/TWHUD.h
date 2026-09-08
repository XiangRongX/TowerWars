// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TWHUD.generated.h"

class UBuildMenu;
class UUpgradeMenu;
class USummonMenu;
class UTWOverlay;
class UTowerDataAsset;

/**
 * 
 */
UCLASS()
class TOWERWARS_API ATWHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void ShowBuildMenu();
	void ShowUpgradeMenu(UTowerDataAsset* TowerData);
	void ShowSummonMenu();

	FORCEINLINE UTWOverlay* GetOverlayWidget() const { return OverlayWidget; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "TW|UI")
	TSubclassOf<UTWOverlay> OverlayClass;

	UPROPERTY(EditDefaultsOnly, Category = "TW|UI")
	TSubclassOf<UBuildMenu> BuildMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "TW|UI")
	TSubclassOf<UUpgradeMenu> UpgradeMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "TW|UI")
	TSubclassOf<USummonMenu> SummonMenuClass;

private:
	TObjectPtr<UTWOverlay> OverlayWidget;
	TObjectPtr<UBuildMenu> BuildMenuWidget;
	TObjectPtr<UUpgradeMenu> UpgradeMenuWidget;
	TObjectPtr<USummonMenu> SummonMenuWidget;
};
