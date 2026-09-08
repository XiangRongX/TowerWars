// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SummonMenu.generated.h"

class UEnemyDataAsset;

/**
 * 
 */
UCLASS()
class TOWERWARS_API USummonMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:


protected:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void HandleSummonOptionSelected(UEnemyDataAsset* SelectedEnemyData);

};
