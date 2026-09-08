// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/PathActor.h"
#include "Components/SplineComponent.h"

APathActor::APathActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	SetRootComponent(PathSpline);
}

void APathActor::BeginPlay()
{
	Super::BeginPlay();
	
}



