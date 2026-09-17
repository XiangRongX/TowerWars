// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/TWGridArea.h"
#include "Grid/TWGridSubsystem.h"

ATWGridArea::ATWGridArea()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATWGridArea::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR

	FlushPersistentDebugLines(GetWorld());

	if (bShowDebugGrid)
	{
		DrawDebugGrid();
	}

#endif
}

void ATWGridArea::BeginPlay()
{
	Super::BeginPlay();

	RegisterToGridSubsystem();
}

void ATWGridArea::RegisterToGridSubsystem()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UTWGridSubsystem* GridSubsystem = World->GetSubsystem<UTWGridSubsystem>();
	if (!GridSubsystem)return;

	GridSubsystem->InitializePlayerGrid(PlayerIndex, GridOrigin, GridSize);
	GridSubsystem->SetBuildableCells(PlayerIndex, BuildableCells);
}

void ATWGridArea::DrawDebugGrid() const
{
#if WITH_EDITOR

	UWorld* World = GetWorld();
	if (!World) return;

	UTWGridSubsystem* GridSubsystem = World->GetSubsystem<UTWGridSubsystem>();
	if (!GridSubsystem)return;

	for (int32 X = 0; X < GridSize.X; ++X)
	{
		for (int32 Y = 0; Y < GridSize.Y; ++Y)
		{
			FIntPoint LocalCoord(X, Y);
			FIntPoint GridCoord = GridOrigin + LocalCoord;
			FVector Center = GridSubsystem->GridToWorldLocation(GridCoord);
			bool bBuildable =	BuildableCells.Contains(LocalCoord);
			FColor Color = bBuildable ? FColor::Green : FColor::Red;

			DrawDebugBox(World, Center, FVector(GridSubsystem->TileSize * 0.5f, GridSubsystem->TileSize * 0.5f, 2.f), Color, true, -1.f, 0, 1.f);
			
			if (bShowDebugText)
			{
				DrawDebugString(World, Center + FVector(0, 0, 10), FString::Printf(TEXT("%d,%d"), GridCoord.X, GridCoord.Y), nullptr, FColor::White, 0.f);
			}
		}
	}

#endif
}
