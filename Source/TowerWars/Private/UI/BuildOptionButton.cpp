// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BuildOptionButton.h"
#include "UI/BuildTooltip.h"
#include "UI/UpgradeTooltip.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/TowerDataAsset.h"
#include "Player/TWPlayerState.h"

void UBuildOptionButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (TooltipClass && TowerData)
	{
		UBuildTooltip* TooltipWidget = CreateWidget<UBuildTooltip>(GetOwningPlayer(), TooltipClass);
		if (TooltipWidget)
		{
			TooltipWidget->SetTooltip(TowerData);
			if (Button_Tower)
			{
				Button_Tower->SetToolTip(TooltipWidget);

				FButtonStyle ButtonStyle = Button_Tower->GetStyle();
				ButtonStyle.Normal.SetResourceObject(TowerData->Icon);
				ButtonStyle.Hovered.SetResourceObject(TowerData->Icon);
				ButtonStyle.Pressed.SetResourceObject(TowerData->Icon);
				ButtonStyle.Disabled.SetResourceObject(TowerData->Icon);
				Button_Tower->SetStyle(ButtonStyle);
				Button_Tower->SetIsEnabled(TowerData->bDefaultUnlocked);
			}
		}

		if (Image_Lock)
		{
			bIsLocked = !TowerData->bDefaultUnlocked;
			Image_Lock->SetVisibility(TowerData->bDefaultUnlocked ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		}
		if(Text_Level)
		{
			Text_Level->SetText(FText::AsNumber(TowerData->Level));
		}
	}

	if (ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>())
	{
		PS->OnGoldChanged.RemoveDynamic(this, &ThisClass::HandleGoldChanged);
		PS->OnGoldChanged.AddDynamic(this, &ThisClass::HandleGoldChanged);

		UpdateAffordability(PS->GetGold());
	}
}

void UBuildOptionButton::NativeDestruct()
{
	Super::NativeDestruct();

	if (ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>())
	{
		PS->OnGoldChanged.RemoveDynamic(this, &ThisClass::HandleGoldChanged);
	}
}

void UBuildOptionButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if(Button_Tower)
	{
		Button_Tower->OnClicked.AddDynamic(this, &ThisClass::OnTowerButtonClicked);
	}
}

void UBuildOptionButton::OnTowerButtonClicked()
{
	if (bIsBuild)
	{
		OnBuildOptionClicked.Broadcast(TowerData);
	}
	else
	{
		OnUpgradeOptionClicked.Broadcast(TowerData);
	}
}

void UBuildOptionButton::HandleGoldChanged(int32 NewGold)
{
	UpdateAffordability(NewGold);
}

void UBuildOptionButton::UpdateAffordability(int32 CurrentGold)
{
	if (!TowerData) return;

	const bool bCanAfford = (CurrentGold >= TowerData->Cost) && !bIsLocked;
	if (Button_Tower)
	{
		Button_Tower->SetIsEnabled(bCanAfford);
	}
}

void UBuildOptionButton::SetLocked(bool bLocked)
{
	bIsLocked = bLocked;
	if (Image_Lock)
	{
		Image_Lock->SetVisibility(bIsLocked ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
}

void UBuildOptionButton::InitUpgradeOption(UTowerDataAsset* Data)
{
	if (TooltipClass && Data)
	{
		TowerData = Data;
		bIsBuild = false;

		UUpgradeTooltip* TooltipWidget = CreateWidget<UUpgradeTooltip>(GetOwningPlayer(), TooltipClass);
		if (TooltipWidget)
		{
			TooltipWidget->SetTooltip(TowerData);
			if (Button_Tower)
			{
				Button_Tower->SetToolTip(TooltipWidget);

				FButtonStyle ButtonStyle = Button_Tower->GetStyle();
				ButtonStyle.Normal.SetResourceObject(TowerData->Icon);
				ButtonStyle.Hovered.SetResourceObject(TowerData->Icon);
				ButtonStyle.Pressed.SetResourceObject(TowerData->Icon);
				ButtonStyle.Disabled.SetResourceObject(TowerData->Icon);
				Button_Tower->SetStyle(ButtonStyle);
				Button_Tower->SetIsEnabled(TowerData->bDefaultUnlocked);
			}
		}

		if (Image_Lock)
		{
			bIsLocked = !TowerData->bDefaultUnlocked;
			Image_Lock->SetVisibility(TowerData->bDefaultUnlocked ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		}
		if (Text_Level)
		{
			Text_Level->SetText(FText::AsNumber(TowerData->Level));
		}
	}
}
