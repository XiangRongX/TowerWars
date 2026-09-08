// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SummonOptionButton.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/SummonTooltip.h"
#include "Data/EnemyDataAsset.h"
#include "Player/TWPlayerState.h"
#include "Player/TWPlayerController.h"

void USummonOptionButton::UpgradeEnemy()
{
	if (!EnemyData || !EnemyData->NextUpgrade) return;

	EnemyData = EnemyData->NextUpgrade;

	if (TooltipClass && EnemyData)
	{
		USummonTooltip* TooltipWidget = CreateWidget<USummonTooltip>(GetOwningPlayer(), TooltipClass);
		if (TooltipWidget)
		{
			//TooltipWidget->SetTooltip(EnemyData);
			if (Button_Enemy)
			{
				Button_Enemy->SetToolTip(TooltipWidget);

				FButtonStyle ButtonStyle = Button_Enemy->GetStyle();
				ButtonStyle.Normal.SetResourceObject(EnemyData->Icon);
				ButtonStyle.Hovered.SetResourceObject(EnemyData->Icon);
				ButtonStyle.Pressed.SetResourceObject(EnemyData->Icon);
				ButtonStyle.Disabled.SetResourceObject(EnemyData->Icon);
				Button_Enemy->SetStyle(ButtonStyle);
			}
		}
	}

	UpdateAffordability();
	UpdateButtonCountText();
}

void USummonOptionButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (TooltipClass && EnemyData)
	{
		USummonTooltip* TooltipWidget = CreateWidget<USummonTooltip>(GetOwningPlayer(), TooltipClass);
		if (TooltipWidget)
		{
			TooltipWidget->SetTooltip(EnemyData);
			if (Button_Enemy)
			{
				Button_Enemy->SetToolTip(TooltipWidget);

				FButtonStyle ButtonStyle = Button_Enemy->GetStyle();
				ButtonStyle.Normal.SetResourceObject(EnemyData->Icon);
				ButtonStyle.Hovered.SetResourceObject(EnemyData->Icon);
				ButtonStyle.Pressed.SetResourceObject(EnemyData->Icon);
				ButtonStyle.Disabled.SetResourceObject(EnemyData->Icon);
				Button_Enemy->SetStyle(ButtonStyle);
				Button_Enemy->SetIsEnabled(EnemyData->bDefaultUnlocked);
			}
		}

		if (Image_Lock)
		{
			bIsLocked = !EnemyData->bDefaultUnlocked;
			Image_Lock->SetVisibility(EnemyData->bDefaultUnlocked ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		}
	}

	ATWPlayerController* PC = GetOwningPlayer<ATWPlayerController>();
	ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>();
	if (PS)
	{
		// 情况 A：PS 已经准备好，直接绑定
		BindPlayerStateEvents(PS);
	}
	else if (PC)
	{
		// 情况 B：PS 尚未同步完成，监听 PC 的就绪广播（只监听一次）
		PC->OnPlayerStateReady.RemoveAll(this);
		PC->OnPlayerStateReady.AddUObject(this, &ThisClass::HandlePlayerStateReady);
	}
}

void USummonOptionButton::NativeDestruct()
{
	if (ATWPlayerController* PC = GetOwningPlayer<ATWPlayerController>())
	{
		PC->OnPlayerStateReady.RemoveAll(this);
	}

	if (ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>())
	{
		PS->OnGoldChanged.RemoveDynamic(this, &ThisClass::HandleGoldChanged);
		PS->OnPlayerStockUpdated.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void USummonOptionButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_Enemy)
	{
		Button_Enemy->OnClicked.AddDynamic(this, &ThisClass::OnSummonnButtonClicked);
	}
}

void USummonOptionButton::UpdateAffordability()
{
	if (!EnemyData) return;

	ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>();
	if (!PS) return;

	// 1. 校验金币是否足够
	const bool bHasEnoughGold = (PS->GetGold() >= EnemyData->Cost);

	// 2. 校验 Stock 是否大于 0
	const bool bHasEnoughStock = (PS->GetCurrentStock() > 0);

	// 3. 综合判断：未锁定 && 金币足够 && Stock > 0
	const bool bCanAfford = bHasEnoughGold && bHasEnoughStock && !bIsLocked;

	if (Button_Enemy)
	{
		Button_Enemy->SetIsEnabled(bCanAfford);
	}
}

void USummonOptionButton::UpdateButtonCountText()
{
	if (Text_Count && EnemyData)
	{
		if (ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>())
		{
			int32 Count = PS->GetCurrentStock();
			Text_Count->SetText(FText::AsNumber(Count));
		}
	}
}

void USummonOptionButton::OnSummonnButtonClicked()
{
	OnSummonOptionClicked.Broadcast(EnemyData);
}

void USummonOptionButton::HandleGoldChanged(int32 NewGold)
{
	UpdateAffordability();
}

void USummonOptionButton::HandleStockUpdated(int32 CurrentStock, int32 MaxStock, const UEnemyDataAsset* UpdatedEnemyData)
{
	UpdateAffordability();

	if(!UpdatedEnemyData || UpdatedEnemyData == EnemyData)
	{
		UpdateButtonCountText();
	}
}

void USummonOptionButton::BindPlayerStateEvents(ATWPlayerState* PS)
{
	if (!PS) return;

	PS->OnGoldChanged.RemoveDynamic(this, &ThisClass::HandleGoldChanged);
	PS->OnGoldChanged.AddDynamic(this, &ThisClass::HandleGoldChanged);

	PS->OnPlayerStockUpdated.RemoveAll(this);
	PS->OnPlayerStockUpdated.AddUObject(this, &ThisClass::HandleStockUpdated);

	// 首次刷新 UI
	UpdateAffordability();
	UpdateButtonCountText();
}


void USummonOptionButton::HandlePlayerStateReady(ATWPlayerState* PS)
{
	if (ATWPlayerController* PC = GetOwningPlayer<ATWPlayerController>())
	{
		PC->OnPlayerStateReady.RemoveAll(this);
	}

	BindPlayerStateEvents(PS);
}
