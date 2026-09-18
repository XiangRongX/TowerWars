// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TowerOverheadWidget.h"
#include "Tower/TWTowerBase.h"
#include "Data/TowerDataAsset.h"
#include "Player/TWPlayerState.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

void UTowerOverheadWidget::InitTower(ATWTowerBase* InTower)
{
	if (!InTower) return;

	Tower = InTower;

	if (APlayerController* PC = GetOwningPlayer())
	{
		PlayerState = PC->GetPlayerState<ATWPlayerState>();

		if (PlayerState.IsValid())
		{
			PlayerState->OnGoldChanged.AddDynamic(this, &UTowerOverheadWidget::OnGoldChanged);
		}
	}

	Refresh();
}

void UTowerOverheadWidget::Refresh()
{
	if (!Tower.IsValid() || !Tower->GetTowerData()) return;

	const UTowerDataAsset* Data = Tower->GetTowerData();

	Text_TowerName->SetText(FText::FromString(FString::Printf(TEXT("%s %d级"), *Data->TowerName.ToString(), Data->Level)));

	UpdateUpgradeVisibility();
}

void UTowerOverheadWidget::UpdateUpgradeVisibility()
{
	if (!Tower.IsValid() || !PlayerState.IsValid() || !Tower->GetTowerData())
	{
		Text_Upgrade->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const UTowerDataAsset* Data = Tower->GetTowerData();

	if (Tower->GetOwnerPlayerIndex() != PlayerState->GetPlayerIndex() || Data->NextUpgrades.Num() == 0)
	{
		Text_Upgrade->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const int32 Gold = PlayerState->GetGold();

	bool bCanUpgrade = false;

	for (const UTowerDataAsset* UpgradeData : Data->NextUpgrades)
	{
		if (UpgradeData && Gold >= UpgradeData->Cost)
		{
			bCanUpgrade = true;
			break;
		}
	}

	Text_Upgrade->SetVisibility(bCanUpgrade ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UTowerOverheadWidget::OnGoldChanged(int32 NewGold)
{
	UpdateUpgradeVisibility();
}
