// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InfoMenu.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Player/TWPlayerState.h"
#include "Game/TWGameState.h"

void UInfoMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UInfoMenu::InitMenu(int32 TotalPlayers, int32 LocalPlayerIndex)
{
	if (!WidgetSwitcher) return;

	HealthTextBlocks.Empty();
	YouTextBlocks.Empty();

	switch (TotalPlayers)
	{
	case 2:
		WidgetSwitcher->SetActiveWidgetIndex(0);
		HealthTextBlocks.Add(TextBlock_Health_2_1);
		HealthTextBlocks.Add(TextBlock_Health_2_2);
		YouTextBlocks.Add(TextBlock_You_2_1);
		YouTextBlocks.Add(TextBlock_You_2_2);
		break;
	case 4:
		WidgetSwitcher->SetActiveWidgetIndex(1);
		HealthTextBlocks.Add(TextBlock_Health_4_1);
		HealthTextBlocks.Add(TextBlock_Health_4_2);
		HealthTextBlocks.Add(TextBlock_Health_4_3);
		HealthTextBlocks.Add(TextBlock_Health_4_4);
		YouTextBlocks.Add(TextBlock_You_4_1);
		YouTextBlocks.Add(TextBlock_You_4_2);
		YouTextBlocks.Add(TextBlock_You_4_3);
		YouTextBlocks.Add(TextBlock_You_4_4);
		break;
	case 6:
		WidgetSwitcher->SetActiveWidgetIndex(2);
		HealthTextBlocks.Add(TextBlock_Health_6_1);
		HealthTextBlocks.Add(TextBlock_Health_6_2);
		HealthTextBlocks.Add(TextBlock_Health_6_3);
		HealthTextBlocks.Add(TextBlock_Health_6_4);
		HealthTextBlocks.Add(TextBlock_Health_6_5);
		HealthTextBlocks.Add(TextBlock_Health_6_6);
		YouTextBlocks.Add(TextBlock_You_6_1);
		YouTextBlocks.Add(TextBlock_You_6_2);
		YouTextBlocks.Add(TextBlock_You_6_3);
		YouTextBlocks.Add(TextBlock_You_6_4);
		YouTextBlocks.Add(TextBlock_You_6_5);
		YouTextBlocks.Add(TextBlock_You_6_6);
		break;
	}

	for(int32 Index = 0; Index < YouTextBlocks.Num(); ++Index)
	{
		UTextBlock* YouText = YouTextBlocks[Index];
		if (!YouText) continue;

		if (Index == LocalPlayerIndex)
		{
			YouText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			YouText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (ATWPlayerState* PS = Cast<ATWPlayerState>(GetOwningPlayerState()))
	{
		BindPlayerStateEvents(PS);
	}
	if (ATWGameState* GS = Cast<ATWGameState>(GetWorld()->GetGameState()))
	{
		BindGameStateEvents(GS);
	}
}

void UInfoMenu::HandlePlayerStateAdded(APlayerState* NewPS)
{
	if (ATWPlayerState* TWPS = Cast<ATWPlayerState>(NewPS))
	{
		BindSinglePlayerState(TWPS);
	}
}

void UInfoMenu::BindPlayerStateEvents(ATWPlayerState* PS)
{
	if (!PS) return;

	PS->OnGoldChanged.RemoveDynamic(this, &UInfoMenu::HandleGoldChanged);
	PS->OnGoldChanged.AddDynamic(this, &UInfoMenu::HandleGoldChanged);

	PS->OnIncomeChanged.RemoveDynamic(this, &UInfoMenu::HandleIncomeChanged);
	PS->OnIncomeChanged.AddDynamic(this, &UInfoMenu::HandleIncomeChanged);

	HandleGoldChanged(PS->GetGold());
	HandleIncomeChanged(PS->GetIncome());

	// 1. 先绑定当前 GameState 中已经存在的 PlayerState
	if (AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr)
	{
		for (APlayerState* PlayerState : GS->PlayerArray)
		{
			if (ATWPlayerState* TWPS = Cast<ATWPlayerState>(PlayerState))
			{
				BindSinglePlayerState(TWPS);
			}
		}
	}
}

void UInfoMenu::BindSinglePlayerState(ATWPlayerState* TWPS)
{
	if (!TWPS) return;

	// 防止重复绑定
	TWPS->OnHealthChanged.RemoveDynamic(this, &UInfoMenu::HandleHealthChanged);
	TWPS->OnHealthChanged.AddDynamic(this, &UInfoMenu::HandleHealthChanged);

	// 绑定后立即用当前血量刷新一次 UI
	int32 CurrentIndex = TWPS->GetPlayerIndex();
	if (HealthTextBlocks.IsValidIndex(CurrentIndex))
	{
		HandleHealthChanged(CurrentIndex, TWPS->GetPlayerHealth());
	}
}

void UInfoMenu::BindGameStateEvents(ATWGameState* GS)
{
	if (!GS) return;

	GS->OnMatchTimerUpdated.AddUObject(this, &UInfoMenu::UpdateGlobalTimerText);
	GS->OnIncomeTimerUpdated.AddUObject(this, &UInfoMenu::UpdateIncomeTimerText);
	UpdateGlobalTimerText(GS->GetMatchTimeRemaining());
	UpdateIncomeTimerText(GS->GetIncomeTimeRemaining());

	GS->OnPlayerStateAdded.RemoveAll(this);
	GS->OnPlayerStateAdded.AddUObject(this, &UInfoMenu::HandlePlayerStateAdded);
}

void UInfoMenu::UpdateGlobalTimerText(int32 TotalSeconds)
{
	if (!TextBlock_Time) return;

	int32 Minutes = TotalSeconds / 60;
	int32 Seconds = TotalSeconds % 60;

	FString FormattedTime = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	TextBlock_Time->SetText(FText::FromString(FormattedTime));
}

void UInfoMenu::UpdateIncomeTimerText(int32 Seconds)
{
	if (!TextBlock_NextIncomeTime) return;

	TextBlock_NextIncomeTime->SetText(FText::AsNumber(Seconds));
}

void UInfoMenu::HandleHealthChanged(int32 PlayerIndex, int32 NewHealth)
{
	if (!HealthTextBlocks.IsValidIndex(PlayerIndex)) return;

	if (HealthTextBlocks[PlayerIndex])
	{
		HealthTextBlocks[PlayerIndex]->SetText(FText::AsNumber(NewHealth));
	}
}

void UInfoMenu::HandleIncomeChanged(int32 NewIncome)
{
	if (TextBlock_Income)
	{
		TextBlock_Income->SetText(FText::AsNumber(NewIncome));
	}
}

void UInfoMenu::HandleGoldChanged(int32 NewGold)
{
	if (TextBlock_Gold)
	{
		TextBlock_Gold->SetText(FText::AsNumber(NewGold));
	}
}
