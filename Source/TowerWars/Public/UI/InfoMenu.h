// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfoMenu.generated.h"

class UWidgetSwitcher;
class UTextBlock;
class ATWPlayerState;
class ATWGameState;

/**
 * 
 */
UCLASS()
class TOWERWARS_API UInfoMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void InitMenu(int32 TotalPlayers, int32 LocalPlayerIndex);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Time;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Gold;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Income;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_NextIncomeTime;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_2_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_2_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_4_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_4_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_4_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_4_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_6_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_6_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_6_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_6_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_6_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Health_6_6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_2_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_2_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_4_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_4_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_4_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_4_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_6_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_6_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_6_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_6_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_6_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_You_6_6;

private:
	TArray<TObjectPtr<UTextBlock>> HealthTextBlocks;
	TArray<TObjectPtr<UTextBlock>> YouTextBlocks;

	void HandlePlayerStateAdded(APlayerState* NewPS);
	void BindPlayerStateEvents(ATWPlayerState* PS);
	void BindSinglePlayerState(ATWPlayerState* TWPS);
	void BindGameStateEvents(ATWGameState* GS);
	void UpdateGlobalTimerText(int32 TotalSeconds);
	void UpdateIncomeTimerText(int32 Seconds);

	UFUNCTION()
	void HandleHealthChanged(int32 PlayerIndex, int32 NewHealth);

	UFUNCTION()
	void HandleGoldChanged(int32 NewGold);

	UFUNCTION()
	void HandleIncomeChanged(int32 NewIncome);
};
