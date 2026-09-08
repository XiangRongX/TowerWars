// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildMenu.generated.h"

class UTowerDataAsset;

/**
 * 
 */
UCLASS()
class TOWERWARS_API UBuildMenu : public UUserWidget
{
	GENERATED_BODY()

public:


protected:
	virtual void NativeOnInitialized() override;
	
private:
	UFUNCTION()
	void HandleTowerOptionSelected(UTowerDataAsset* SelectedTowerData);
};
