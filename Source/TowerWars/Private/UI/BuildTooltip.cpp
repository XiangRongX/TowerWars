// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BuildTooltip.h"
#include "Data/TowerDataAsset.h"
#include "Components/TextBlock.h"

void UBuildTooltip::SetTooltip(const UTowerDataAsset* Data)
{
	Text_Name->SetText(FText::FromString(FString::Printf(TEXT("%s %d级"), *Data->TowerName.ToString(), Data->Level)));
	Text_Description->SetText(Data->Description);
	Text_Damage->SetText(FText::FromString(FString::Printf(TEXT("攻击伤害：%.0f"), Data->Damage)));
	Text_Range->SetText(FText::FromString(FString::Printf(TEXT("攻击范围：%.0f"), Data->AttackRange)));
	Text_Interval->SetText(FText::FromString(FString::Printf(TEXT("攻击间歇：%.1f"), Data->AttackInterval)));
	Text_Cost->SetText(FText::FromString(FString::Printf(TEXT("花费：%d"), Data->Cost)));
	Text_Special->SetText(FText::FromString(FString::Printf(TEXT("特殊：%s"), *Data->Special.ToString())));
}
