// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TWGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOWERWARS_API UTWGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TW|Lobby")
	int32 TargetTotalPlayers = 4;
};
