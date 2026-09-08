// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeTooltip.generated.h"

class UTowerDataAsset;

/**
 * 
 */
UCLASS()
class TOWERWARS_API UUpgradeTooltip : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTooltip(const UTowerDataAsset* Data);

protected:

};
