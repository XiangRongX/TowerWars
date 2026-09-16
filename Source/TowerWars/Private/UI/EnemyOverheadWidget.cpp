// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EnemyOverheadWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Player/TWPlayerState.h"
#include "Enemy/TWEnemyBase.h"

void UEnemyOverheadWidget::InitEnemy(ATWEnemyBase* InEnemy)
{
	if (!InEnemy) return;

	Enemy = InEnemy;
	ShowAliveState();
}

void UEnemyOverheadWidget::ShowAliveState()
{
	if (!Enemy.IsValid()) return;

	if (ATWPlayerState* SummonerPS = Enemy->GetSummonerPlayerState())
	{
		Text_Player->SetText(FText::FromString(SummonerPS->GetPlayerName()));
		Text_Player->SetColorAndOpacity(GetPlayerColor(SummonerPS->GetPlayerIndex()));
	}

	Text_Health->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Enemy->GetHealth())));

	Box_DeathReward->SetVisibility(ESlateVisibility::Collapsed);
	Box_Info->SetVisibility(ESlateVisibility::HitTestInvisible);

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UEnemyOverheadWidget::ShowDeathReward(int32 GoldReward, int32 IncomeReward)
{
	Box_Info->SetVisibility(ESlateVisibility::Collapsed);
	Box_DeathReward->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (GoldReward > 0)
	{
		Text_Gold->SetText(FText::FromString(FString::Printf(TEXT("+%d 金币"), GoldReward)));
		Text_Gold->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Text_Gold->SetVisibility(ESlateVisibility::Collapsed);
	}

	if(IncomeReward > 0)
	{
		Text_Income->SetText(FText::FromString(FString::Printf(TEXT(" ( +%d 收入 )"), IncomeReward)));
		Text_Income->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Text_Income->SetVisibility(ESlateVisibility::Collapsed);
	}
}
