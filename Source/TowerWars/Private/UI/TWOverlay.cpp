// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TWOverlay.h"
#include "UI/InfoMenu.h"
#include "Player/TWPlayerState.h"
#include "Game/TWGameState.h"

void UTWOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	TryInitInfoMenu();
}

void UTWOverlay::TryInitInfoMenu()
{
	if (WBP_InfoMenu)
	{
		ATWPlayerState* PS = Cast<ATWPlayerState>(GetOwningPlayerState());
		ATWGameState* GS = Cast<ATWGameState>(GetWorld()->GetGameState());
		if (PS && GS)
		{
			WBP_InfoMenu->InitMenu(GS->TargetTotalPlayers, PS->GetPlayerIndex());
			SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}
