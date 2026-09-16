// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SummonTooltip.generated.h"

class UTextBlock;
class UEnemyDataAsset;

/**
 * 
 */
UCLASS()
class TOWERWARS_API USummonTooltip : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTooltip(const UEnemyDataAsset* Data);
	void SetStock(int32 NewStock);
	void SetSummoned(int32 NewSummoned);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Description;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Health;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Speed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Cost;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Income;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Stock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Summoned;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Special;

private:
	TWeakObjectPtr<const UEnemyDataAsset> BoundEnemyData;

	// 收到 Stock/Summoned 广播时的回调
	void HandleStockUpdated(int32 CurrentStock, int32 MaxStock, const UEnemyDataAsset* UpdatedEnemyData);
	// 全局怪物血量倍率变化回调
	void HandleEnemyHealthMultiplierChanged(float NewMultiplier);

	// 刷新全局/个人 Stock 与 Summoned 数值
	void RefreshStockAndSummoned();
	// 刷新当前 Tooltip 显示的怪物最大血量
	void RefreshEnemyHealth();
};
