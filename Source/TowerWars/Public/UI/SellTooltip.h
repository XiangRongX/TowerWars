// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SellTooltip.generated.h"

class UTextBlock;
class UTowerDataAsset;

/**
 * 
 */
UCLASS()
class TOWERWARS_API USellTooltip : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTooltip(const UTowerDataAsset* Data);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Sell;

};
