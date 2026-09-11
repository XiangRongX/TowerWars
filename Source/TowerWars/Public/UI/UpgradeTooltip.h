// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BuildTooltip.h"
#include "UpgradeTooltip.generated.h"

class UTowerDataAsset;

/**
 * 
 */
UCLASS()
class TOWERWARS_API UUpgradeTooltip : public UBuildTooltip
{
	GENERATED_BODY()
	
public:
	void SetUpgradeTooltip(const UTowerDataAsset* CurrentData, const UTowerDataAsset* TargetData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_NewCost;
};
