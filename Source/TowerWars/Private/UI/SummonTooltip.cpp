// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SummonTooltip.h"
#include "Data/EnemyDataAsset.h"
#include "Components/TextBlock.h"
#include "Player/TWPlayerState.h"

void USummonTooltip::SetTooltip(const UEnemyDataAsset* Data)
{
	if (!Data) return;

	BoundEnemyData = Data;

	Text_Name->SetText(FText::FromString(FString::Printf(TEXT("%s"), *Data->EnemyName.ToString())));
	Text_Description->SetText(Data->Description);
	Text_Health->SetText(FText::FromString(FString::Printf(TEXT("血量：%.0f"), Data->Health)));
	Text_Speed->SetText(FText::FromString(FString::Printf(TEXT("速度：%.0f"), Data->Speed)));
	Text_Cost->SetText(FText::FromString(FString::Printf(TEXT("花费：%d"), Data->Cost)));
	Text_Income->SetText(FText::FromString(FString::Printf(TEXT("收入：%d"), Data->Income)));
	Text_Special->SetText(FText::FromString(FString::Printf(TEXT("特殊：%s"), *Data->Special.ToString())));

	RefreshStockAndSummoned();
}

void USummonTooltip::SetStock(int32 NewStock)
{
	Text_Stock->SetText(FText::FromString(FString::Printf(TEXT("库存：%d"), NewStock)));
}

void USummonTooltip::SetSummoned(int32 NewSummoned)
{
	Text_Stock->SetText(FText::FromString(FString::Printf(TEXT("已召唤：%d"), NewSummoned)));
}

void USummonTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	if (ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>())
	{
		PS->OnPlayerStockUpdated.AddUObject(this, &ThisClass::HandleStockUpdated);
	}

	RefreshStockAndSummoned();
}

void USummonTooltip::NativeDestruct()
{
	Super::NativeDestruct();

	if (ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>())
	{
		PS->OnPlayerStockUpdated.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void USummonTooltip::HandleStockUpdated(int32 CurrentStock, int32 MaxStock, const UEnemyDataAsset* UpdatedEnemyData)
{
	if (!UpdatedEnemyData || UpdatedEnemyData == BoundEnemyData)
	{
		SetStock(CurrentStock);

		if (BoundEnemyData.IsValid())
		{
			if (ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>())
			{
				SetSummoned(PS->GetSummonedCountForEnemy(BoundEnemyData.Get()));
			}
		}
	}
}

void USummonTooltip::RefreshStockAndSummoned()
{
	if (ATWPlayerState* PS = GetOwningPlayerState<ATWPlayerState>())
	{
		SetStock(PS->GetCurrentStock());

		if (BoundEnemyData.IsValid())
		{
			SetSummoned(PS->GetSummonedCountForEnemy(BoundEnemyData.Get()));
		}
	}
}
