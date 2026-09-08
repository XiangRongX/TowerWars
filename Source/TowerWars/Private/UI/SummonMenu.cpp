// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SummonMenu.h"
#include "UI/SummonOptionButton.h"
#include "Player/TWPlayerController.h"
#include "Blueprint/WidgetTree.h"

void USummonMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TArray<USummonOptionButton*> OptionButtons;
	WidgetTree->ForEachWidget([&OptionButtons](UWidget* Widget) {
		if (USummonOptionButton* OptionButton = Cast<USummonOptionButton>(Widget))
		{
			OptionButtons.Add(OptionButton);
		}
		});

	for (USummonOptionButton* OptionButton : OptionButtons)
	{
		OptionButton->OnSummonOptionClicked.AddDynamic(this, &ThisClass::HandleSummonOptionSelected);
	}
}

void USummonMenu::HandleSummonOptionSelected(UEnemyDataAsset* SelectedEnemyData)
{
	if (!SelectedEnemyData) return;

	ATWPlayerController* PC = GetOwningPlayer<ATWPlayerController>();
	if (PC && PC->IsLocalController())
	{
		PC->RequestSummonEnemy(SelectedEnemyData);
		FInputModeGameAndUI InputMode;
		PC->SetInputMode(InputMode);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}
