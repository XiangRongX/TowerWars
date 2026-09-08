#pragma once

#include "CoreMinimal.h"
#include "TWType.generated.h"

UENUM(BlueprintType)
enum class ETowerTargetStrategy : uint8
{
	First       UMETA(DisplayName = "First (离终点最近)"),
	Last        UMETA(DisplayName = "Last (离起点最近)"),
	Strongest   UMETA(DisplayName = "Strongest (血量最高)"),
	Weakest     UMETA(DisplayName = "Weakest (残血优先)"),
	Closest     UMETA(DisplayName = "Closest (离塔最近)")
};

UENUM(BlueprintType)
enum class ETowerDamageType : uint8
{
	SingleTarget  UMETA(DisplayName = "单体即时"),
	AreaOfEffect  UMETA(DisplayName = "范围即时 (AoE)")
};
