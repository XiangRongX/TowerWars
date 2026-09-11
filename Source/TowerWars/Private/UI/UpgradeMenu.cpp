// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UpgradeMenu.h"
#include "Data/TowerDataAsset.h"
#include "UI/BuildOptionButton.h"
#include "UI/SellButton.h"
#include "Player/TWPlayerController.h"

void UUpgradeMenu::InitUpgradeMenu(UTowerDataAsset* TowerData)
{
	if (!TowerData) return;

	switch (TowerData->NextUpgrades.Num())
	{
	case 0:
		WBP_BuildOptionButton1->SetVisibility(ESlateVisibility::Collapsed);
		WBP_BuildOptionButton2->SetVisibility(ESlateVisibility::Collapsed);
		break;

	case 1:
		WBP_BuildOptionButton1->SetVisibility(ESlateVisibility::Visible);
		WBP_BuildOptionButton1->InitUpgradeOption(TowerData->NextUpgrades[0], TowerData);
		WBP_BuildOptionButton2->SetVisibility(ESlateVisibility::Collapsed);
		break;

	case 2:
		WBP_BuildOptionButton1->SetVisibility(ESlateVisibility::Visible);
		WBP_BuildOptionButton1->InitUpgradeOption(TowerData->NextUpgrades[0], TowerData);
		WBP_BuildOptionButton2->SetVisibility(ESlateVisibility::Visible);
		WBP_BuildOptionButton2->InitUpgradeOption(TowerData->NextUpgrades[1], TowerData);
		break;
	}

	WBP_SellButton->InitSellButton(TowerData);
}

void UUpgradeMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if(WBP_BuildOptionButton1)
	{
		WBP_BuildOptionButton1->OnUpgradeOptionClicked.AddDynamic(this, &ThisClass::HandleTowerOptionSelected);
	}
	if(WBP_BuildOptionButton2)
	{
		WBP_BuildOptionButton2->OnUpgradeOptionClicked.AddDynamic(this, &ThisClass::HandleTowerOptionSelected);
	}
	if (WBP_SellButton)
	{
		WBP_SellButton->OnSellClicked.AddDynamic(this, &ThisClass::HandleSellSelected);
	}
}

void UUpgradeMenu::HandleTowerOptionSelected(UTowerDataAsset* SelectedTowerData)
{
	if (!SelectedTowerData) return;

	ATWPlayerController* PC = GetOwningPlayer<ATWPlayerController>();
	if (PC && PC->IsLocalController())
	{
		PC->RequestUpgradeTower(SelectedTowerData);
		FInputModeGameAndUI InputMode;
		PC->SetInputMode(InputMode);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

void UUpgradeMenu::HandleSellSelected(UTowerDataAsset* SelectedTowerData)
{
	if (!SelectedTowerData) return;

	ATWPlayerController* PC = GetOwningPlayer<ATWPlayerController>();
	if (PC && PC->IsLocalController())
	{
		PC->RequestSellTower();
		FInputModeGameAndUI InputMode;
		PC->SetInputMode(InputMode);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}
