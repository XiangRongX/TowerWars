// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UpgradeTooltip.h"
#include "Data/TowerDataAsset.h"
#include "Components/TextBlock.h"

void UUpgradeTooltip::SetUpgradeTooltip(const UTowerDataAsset* CurrentData, const UTowerDataAsset* TargetData)
{
	if (!TargetData) return;

	// 1. 先调用基础属性展示（填充 TargetData 的攻击力、射程等）
	SetTooltip(TargetData);

	// 2. 计算升级所需差价
	const int32 CurrentCost = CurrentData ? CurrentData->Cost : 0;
	const int32 CostDifference = FMath::Max(0, TargetData->Cost - CurrentCost);

	// 3. 填充 UI 文本
	if (Text_Cost)
	{
		// 显示目标塔的总造价/原价
		Text_Cost->SetText(FText::AsNumber(TargetData->Cost));
	}

	if (Text_NewCost)
	{
		// 显示升级本次实际需消耗的差价金币
		Text_NewCost->SetText(FText::AsNumber(CostDifference));
	}
}
