// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BuildMenu.h"
#include "UI/BuildOptionButton.h"
#include "Player/TWPlayerController.h"
#include "Blueprint/WidgetTree.h"

void UBuildMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TArray<UBuildOptionButton*> OptionButtons;
	WidgetTree->ForEachWidget([&OptionButtons](UWidget* Widget) {
		if (UBuildOptionButton* OptionButton = Cast<UBuildOptionButton>(Widget))
		{
			OptionButtons.Add(OptionButton);
		}
		});

	for (UBuildOptionButton* OptionButton : OptionButtons)
	{
		OptionButton->OnBuildOptionClicked.AddDynamic(this, &ThisClass::HandleTowerOptionSelected);
	}
}

void UBuildMenu::HandleTowerOptionSelected(UTowerDataAsset* SelectedTowerData)
{
	if (!SelectedTowerData) return;

	ATWPlayerController* PC = GetOwningPlayer<ATWPlayerController>();
	if (PC && PC->IsLocalController())
	{
		PC->RequestBuildTower(SelectedTowerData);
		FInputModeGameAndUI InputMode;
		PC->SetInputMode(InputMode);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}
