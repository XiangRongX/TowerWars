// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TWGridArea.generated.h"

class UBoxComponent;

UCLASS()
class TOWERWARS_API ATWGridArea : public AActor
{
	GENERATED_BODY()

public:
	ATWGridArea();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "TW|Grid Area")
	int32 PlayerIndex = 0;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "TW|Grid Area")
	FIntPoint GridOrigin = FIntPoint::ZeroValue;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "TW|Grid Area")
	FIntPoint GridSize = FIntPoint(20, 30);

	/**
	 * 区域内禁止建造的格子
	 *
	 * 坐标是相对于 GridOrigin 的局部坐标
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "TW|Grid Area")
	TArray<FIntPoint> BuildableCells;

	/**
	 * 是否在编辑器里显示区域
	 */
	UPROPERTY(EditInstanceOnly, Category = "TW|Grid Area|Debug")
	bool bShowDebugGrid = true;

	/**
	 * 是否显示坐标
	 */
	UPROPERTY(EditInstanceOnly, Category = "TW|Grid Area|Debug")
	bool bShowDebugText = false;

private:
	void RegisterToGridSubsystem();
	void DrawDebugGrid() const;
};
