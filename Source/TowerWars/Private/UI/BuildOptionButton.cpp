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
			}
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

		PS->OnIncomeChanged.RemoveDynamic(this, &ThisClass::HandleIncomeChanged);
		PS->OnIncomeChanged.AddDynamic(this, &ThisClass::HandleIncomeChanged);

		RefreshState();
	}
}

void UBuildOptionButton::NativeDestruct()
{
	if (ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>())
	{
		PS->OnGoldChanged.RemoveDynamic(this, &ThisClass::HandleGoldChanged);
		PS->OnIncomeChanged.RemoveDynamic(this, &ThisClass::HandleIncomeChanged);
	}

	Super::NativeDestruct();
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
	RefreshState();
}

void UBuildOptionButton::HandleIncomeChanged(int32 NewIncome)
{
	RefreshState();
}

void UBuildOptionButton::RefreshState()
{
	if (!TowerData) return;

	ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>();
	if (!PS) return;

	const int32 CurrentIncome = PS->GetIncome();
	const int32 CurrentGold = PS->GetGold();

	// 1. 判断是否解锁 (默认解锁 OR 收入达到 Cost/5)
	bIsLocked = !TowerData->IsUnlocked(CurrentIncome);

	if (Image_Lock)
	{
		Image_Lock->SetVisibility(bIsLocked ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	// 2. 按钮只有在【已解锁】且【金币足够】时才允许点击
	const bool bCanAfford = !bIsLocked && (CurrentGold >= TowerData->Cost);
	if (Button_Tower)
	{
		Button_Tower->SetIsEnabled(bCanAfford);
	}
}

void UBuildOptionButton::UpdateAffordability(int32 CurrentGold)
{
	RefreshState();
}

void UBuildOptionButton::SetLocked(bool bLocked)
{
	bIsLocked = bLocked;
	if (Image_Lock)
	{
		Image_Lock->SetVisibility(bIsLocked ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
	RefreshState();
}

void UBuildOptionButton::InitUpgradeOption(UTowerDataAsset* TargetData, UTowerDataAsset* CurrentData)
{
	if (TooltipClass && TargetData)
	{
		TowerData = TargetData;
		bIsBuild = false;

		UUpgradeTooltip* TooltipWidget = CreateWidget<UUpgradeTooltip>(GetOwningPlayer(), TooltipClass);
		if (TooltipWidget)
		{
			TooltipWidget->SetUpgradeTooltip(CurrentData, TargetData);
			if (Button_Tower)
			{
				Button_Tower->SetToolTip(TooltipWidget);

				FButtonStyle ButtonStyle = Button_Tower->GetStyle();
				ButtonStyle.Normal.SetResourceObject(TowerData->Icon);
				ButtonStyle.Hovered.SetResourceObject(TowerData->Icon);
				ButtonStyle.Pressed.SetResourceObject(TowerData->Icon);
				ButtonStyle.Disabled.SetResourceObject(TowerData->Icon);
				Button_Tower->SetStyle(ButtonStyle);
			}
		}

		if (Text_Level)
		{
			Text_Level->SetText(FText::AsNumber(TowerData->Level));
		}

		RefreshState();
	}
}
