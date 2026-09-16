// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TWHUD.h"
#include "UI/BuildMenu.h"
#include "UI/UpgradeMenu.h"
#include "UI/SummonMenu.h"
#include "UI/TWOverlay.h"

void ATWHUD::ShowBuildMenu()
{
	if(BuildMenuWidget)
	{
		BuildMenuWidget->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		GetOwningPlayerController()->SetInputMode(InputMode);
	}
}

void ATWHUD::ShowUpgradeMenu(UTowerDataAsset* TowerData)
{
	if (UpgradeMenuWidget)
	{
		UpgradeMenuWidget->SetVisibility(ESlateVisibility::Visible);
		UpgradeMenuWidget->InitUpgradeMenu(TowerData);

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		GetOwningPlayerController()->SetInputMode(InputMode);
	}
}

void ATWHUD::ShowSummonMenu()
{
	if (SummonMenuWidget)
	{
		SummonMenuWidget->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		GetOwningPlayerController()->SetInputMode(InputMode);
	}
}

void ATWHUD::BeginPlay()
{
	Super::BeginPlay();

	if (OverlayClass)
	{
		OverlayWidget = CreateWidget<UTWOverlay>(GetWorld(), OverlayClass);
		if (OverlayWidget)
		{
			OverlayWidget->AddToViewport();
		}
	}
	if(BuildMenuClass)
	{
		BuildMenuWidget = CreateWidget<UBuildMenu>(GetWorld(), BuildMenuClass);
		if(BuildMenuWidget)
		{
			BuildMenuWidget->AddToViewport();
			BuildMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if(UpgradeMenuClass)
	{
		UpgradeMenuWidget = CreateWidget<UUpgradeMenu>(GetWorld(), UpgradeMenuClass);
		if(UpgradeMenuWidget)
		{
			UpgradeMenuWidget->AddToViewport();
			UpgradeMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (SummonMenuClass)
	{
		SummonMenuWidget = CreateWidget<USummonMenu>(GetWorld(), SummonMenuClass);
		if (SummonMenuWidget)
		{
			SummonMenuWidget->AddToViewport();
			SummonMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
