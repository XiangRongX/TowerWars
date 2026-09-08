// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathActor.generated.h"

class USplineComponent;

UCLASS()
class TOWERWARS_API APathActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APathActor();

	FORCEINLINE USplineComponent* GetSplineComponent() const { return PathSpline; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TW|Path")
	TObjectPtr<USplineComponent> PathSpline;
};
