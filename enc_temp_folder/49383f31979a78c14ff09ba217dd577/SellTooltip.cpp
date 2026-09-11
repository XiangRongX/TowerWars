// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SellTooltip.h"
#include "Data/TowerDataAsset.h"
#include "Components/TextBlock.h"

void USellTooltip::SetTooltip(const UTowerDataAsset* Data)
{
	Text_Name->SetText(FText::FromString(FString::Printf(TEXT("出售 %s %d级"), *Data->TowerName.ToString(), Data->Level)));
	Text_Sell->SetText(FText::FromString(FString::Printf(TEXT("回收金额：%d"), Data->Sell)));
}
