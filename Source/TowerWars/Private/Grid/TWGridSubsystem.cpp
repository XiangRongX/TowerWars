// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/TWGridSubsystem.h"
#include "Tower/TWTowerBase.h"

void UTWGridSubsystem::InitializePlayerGrid(int32 PlayerId, FIntPoint StartCoord, FIntPoint GridSize)
{
	for (int32 x = 0; x < GridSize.X; ++x)
	{
		for (int32 y = 0; y < GridSize.Y; ++y)
		{
			FIntPoint Coord = StartCoord + FIntPoint(x, y);

			FGridCell Cell;
			Cell.GridCoords = Coord;
			Cell.OwnerPlayerId = PlayerId;
			Cell.CellType = EGridCellType::Empty;

			GridCells.Add(Coord, Cell);
		}
	}
}

FIntPoint UTWGridSubsystem::WorldToGridCoords(const FVector& WorldLocation) const
{
	int32 X = FMath::FloorToInt(WorldLocation.X / TileSize);
	int32 Y = FMath::FloorToInt(WorldLocation.Y / TileSize);
	return FIntPoint(X, Y);
}

FVector UTWGridSubsystem::GridToWorldLocation(const FIntPoint& GridCoords) const
{
	float X = (GridCoords.X * TileSize) + (TileSize * 0.5f);
	float Y = (GridCoords.Y * TileSize) + (TileSize * 0.5f);
	return FVector(X, Y, GroundZ);
}

bool UTWGridSubsystem::CanBuildAt(int32 PlayerId, const FIntPoint& GridCoords) const
{
	const FGridCell* Cell = GridCells.Find(GridCoords);
	if (!Cell)
	{
		return false; // 超出地图边界
	}

	// 必须是自己的领地，且必须是空地
	return (Cell->OwnerPlayerId == PlayerId) && (Cell->CellType == EGridCellType::Empty);
}

bool UTWGridSubsystem::OccupyCell(int32 PlayerId, const FIntPoint& GridCoords, ATWTowerBase* Tower)
{
	if (!CanBuildAt(PlayerId, GridCoords))
	{
		return false;
	}

	// 更新格子状态为已阻挡
	FGridCell& Cell = GridCells[GridCoords];
	Cell.CellType = EGridCellType::Built;
	Cell.PlacedTower = Tower;
	return true;
}

const FGridCell* UTWGridSubsystem::GetCellData(const FIntPoint& GridCoords) const
{
	return GridCells.Find(GridCoords);
}

bool UTWGridSubsystem::ReleaseCell(int32 PlayerId, const FIntPoint& GridCoords)
{
	FGridCell& Cell = GridCells[GridCoords];
	Cell.CellType = EGridCellType::Empty;
	Cell.PlacedTower = nullptr;

	return true;
}

