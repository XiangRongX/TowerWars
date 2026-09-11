// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SellButton.h"
#include "Components/Button.h"
#include "UI/SellTooltip.h"
#include "Data/TowerDataAsset.h"

void USellButton::NativeConstruct()
{
	Super::NativeConstruct();

}

void USellButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if(Button_Sell)
	{
		Button_Sell->OnClicked.AddDynamic(this, &ThisClass::OnSellButtonClicked);
	}
}

void USellButton::OnSellButtonClicked()
{
	OnSellClicked.Broadcast(TowerData);
}

void USellButton::InitSellButton(UTowerDataAsset* Data)
{
	TowerData = Data;

	if (TooltipClass && TowerData)
	{
		USellTooltip* TooltipWidget = CreateWidget<USellTooltip>(GetOwningPlayer(), TooltipClass);
		if (TooltipWidget)
		{
			TooltipWidget->SetTooltip(TowerData);
		}
	}
}
