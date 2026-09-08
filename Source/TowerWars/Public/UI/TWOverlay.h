// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TWOverlay.generated.h"

class UInfoMenu;

/**
 * 
 */
UCLASS()
class TOWERWARS_API UTWOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void TryInitInfoMenu();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInfoMenu> WBP_InfoMenu;

private:

};
