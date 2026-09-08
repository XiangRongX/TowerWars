// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TWGridSubsystem.generated.h"

class ATWTowerBase;

UENUM(BlueprintType)
enum class EGridCellType :uint8
{
	Empty,
	Built,
	Blocked,
	Path
};

USTRUCT(BlueprintType)
struct FGridCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint GridCoords = FIntPoint::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OwnerPlayerId = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGridCellType CellType = EGridCellType::Empty;

	TWeakObjectPtr<ATWTowerBase> PlacedTower = nullptr;
};

/**
 * 
 */
UCLASS()
class TOWERWARS_API UTWGridSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "TW|Grid")
	float TileSize = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "TW|Grid")
	float GroundZ = 0.f;

	UFUNCTION(BlueprintCallable, Category = "TW|Grid")
	void InitializePlayerGrid(int32 PlayerId, FIntPoint StartCoord, FIntPoint GridSize);

	UFUNCTION(BlueprintCallable, Category = "TW|Grid")
	FIntPoint WorldToGridCoords(const FVector& WorldLocation) const;

	UFUNCTION(BlueprintCallable, Category = "TW|Grid")
	FVector GridToWorldLocation(const FIntPoint& GridCoords) const;

	UFUNCTION(BlueprintCallable, Category = "TW|Grid")
	bool CanBuildAt(int32 PlayerId, const FIntPoint& GridCoords) const;

	UFUNCTION(BlueprintCallable, Category = "TW|Grid")
	bool OccupyCell(int32 PlayerId, const FIntPoint& GridCoords, ATWTowerBase* Tower);

	const FGridCell* GetCellData(const FIntPoint& GridCoords) const;
	bool ReleaseCell(int32 PlayerId, const FIntPoint& GridCoords);

private:
	TMap<FIntPoint, FGridCell> GridCells;
	
};
