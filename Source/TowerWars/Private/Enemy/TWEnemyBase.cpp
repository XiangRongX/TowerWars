// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TWEnemyBase.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Data/EnemyDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "Player/TWPlayerState.h"
#include "Game/TWGameState.h"
#include "Components/WidgetComponent.h"
#include "UI/EnemyOverheadWidget.h"

ATWEnemyBase::ATWEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(false);
	SetNetUpdateFrequency(10.f);

	HitCollision = CreateDefaultSubobject<USphereComponent>(TEXT("HitCollision"));
	SetRootComponent(HitCollision);
	HitCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	EnemyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EnemyMesh"));
	EnemyMesh->SetupAttachment(RootComponent);
	EnemyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidgetComponent->SetupAttachment(RootComponent);
	OverheadWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	OverheadWidgetComponent->SetDrawAtDesiredSize(true);
	OverheadWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
}

void ATWEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (OverheadWidgetComponent && OverheadWidgetClass)
	{
		OverheadWidgetComponent->SetWidgetClass(OverheadWidgetClass);
		if (UEnemyOverheadWidget* Widget = Cast<UEnemyOverheadWidget>(OverheadWidgetComponent->GetUserWidgetObject()))
		{
			Widget->InitEnemy(this);
		}
	}
}

void ATWEnemyBase::OnHealthChanged(float NewHealth, float DamageAmount)
{
	if (OverheadWidgetComponent)
	{
		if (UEnemyOverheadWidget* Widget = Cast<UEnemyOverheadWidget>(OverheadWidgetComponent->GetUserWidgetObject()))
		{
			Widget->ShowAliveState();
		}
	}
}

void ATWEnemyBase::DestroyAfterDeathReward()
{
	Destroy();
}

void ATWEnemyBase::OnEnemyDeath()
{
	if (!HasAuthority() || bDeathHandled) return;

	bDeathHandled = true;

	int32 GoldReward = 0;
	int32 IncomeReward = 0;

	// 寻找跑道对应的玩家 PlayerState
	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		for (APlayerState* PS : GS->PlayerArray)
		{
			if (ATWPlayerState* TWPS = Cast<ATWPlayerState>(PS))
			{
				if (TWPS->GetPlayerIndex() == TargetPlayerIndex)
				{
					const int32 CurrentStar = TWPS->GetEnemyStarLevel(EnemyData.Get());
					const int32 ActualCost = EnemyData->GetCostForStar(CurrentStar);
					const int32 ActualIncome = EnemyData->GetIncomeForStar(CurrentStar);
					GoldReward = FMath::RoundToInt(ActualCost * 0.17f);
					IncomeReward = FMath::RoundToInt(ActualIncome * 0.02f);
					TWPS->AddGold(GoldReward);
					TWPS->AddIncome(IncomeReward);
					break;
				}
			}
		}
	}

	Multicast_ShowDeathReward(GoldReward, IncomeReward);

	GetWorldTimerManager().SetTimer(DeathDestroyTimerHandle, this, &ATWEnemyBase::DestroyAfterDeathReward, 2.0f, false);
}

void ATWEnemyBase::HandleReachedGoal()
{
	if (!HasAuthority() || !IsAlive()) return;

	ATWPlayerState* TargetPS = nullptr;

	// 1. 根据 TargetPlayerIndex 找到跑道所属的防守方 PlayerState
	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		for (APlayerState* PS : GS->PlayerArray)
		{
			if (ATWPlayerState* TWPS = Cast<ATWPlayerState>(PS))
			{
				if (TWPS->GetPlayerIndex() == TargetPlayerIndex)
				{
					TargetPS = TWPS;
					break;
				}
			}
		}
	}

	// 2. 防守方（被拆塔/进怪的人）扣血
	if (TargetPS)
	{
		TargetPS->ReduceHealth(HeartValue);
	}

	// 3. 进攻方（召唤者）偷血：增加防守方被扣除的血量
	if (SummonerPlayerState && SummonerPlayerState != TargetPS)
	{
		SummonerPlayerState->AddHealth(HeartValue);
	}

	// 4. 完成结算后销毁怪物
	Destroy();
}

void ATWEnemyBase::OnRep_PathSpline()
{
	RecalculateLocationFromNetwork();
}

void ATWEnemyBase::OnRep_Health(float OldHealth)
{
	float DamageTaken = OldHealth - Health;

	if (Health > 0.0f)
	{
		OnHealthChanged(Health, DamageTaken);
	}
}

void ATWEnemyBase::OnRep_Speed()
{
	RecalculateLocationFromNetwork();
}

void ATWEnemyBase::Multicast_ShowDeathReward_Implementation(int32 GoldReward, int32 IncomeReward)
{
	EnemyMesh->SetVisibility(false, true);
	HitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!OverheadWidgetComponent) return;

	if (UEnemyOverheadWidget* Widget = Cast<UEnemyOverheadWidget>(OverheadWidgetComponent->GetUserWidgetObject()))
	{
		Widget->ShowDeathReward(GoldReward, IncomeReward);
	}
}

void ATWEnemyBase::RecalculateLocationFromNetwork()
{
	if (!PathSpline || ServerSpawnTime <= 0.f) return;

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (!GS) return;

	// 1. 计算从服务器生成到现在过去了多久（包含网络传输延迟时间）
	float CurrentServerTime = GS->GetServerWorldTimeSeconds();
	float ElapsedTime = FMath::Max(0.0f, CurrentServerTime - ServerSpawnTime);

	// 2. 追赶推算距离：距离 = 时间 × 速度
	DistanceAlongSpline = ElapsedTime * Speed;

	// 3. 立即更新当前渲染坐标
	FVector CenterLocation = PathSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	FVector RightVector = PathSpline->GetRightVectorAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	FRotator WorldRotation = PathSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	SetActorLocationAndRotation(CenterLocation + (RightVector * BaseLateralOffset), WorldRotation);
}

void ATWEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PathSpline || !IsAlive()) return;

	DistanceAlongSpline += Speed * DeltaTime;

	FTransform SplineTransform = PathSpline->GetTransformAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	FVector CenterLocation = SplineTransform.GetLocation();
	FRotator WorldRotation = SplineTransform.GetRotation().Rotator();
	FVector RightVector = SplineTransform.GetRotation().GetRightVector();
	FVector FinalLocation = CenterLocation + (RightVector * BaseLateralOffset);
	SetActorLocationAndRotation(FinalLocation, WorldRotation);

	if (HasAuthority() && DistanceAlongSpline >= PathSpline->GetSplineLength())
	{
		HandleReachedGoal();
	}
}

void ATWEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATWEnemyBase, PathSpline);
	DOREPLIFETIME(ATWEnemyBase, TargetPlayerIndex);
	DOREPLIFETIME(ATWEnemyBase, MaxHealth);
	DOREPLIFETIME(ATWEnemyBase, Health);
	DOREPLIFETIME(ATWEnemyBase, Speed);
	DOREPLIFETIME(ATWEnemyBase, BaseLateralOffset);
	DOREPLIFETIME(ATWEnemyBase, ServerSpawnTime);
	DOREPLIFETIME(ATWEnemyBase, SummonerPlayerState);
	DOREPLIFETIME(ATWEnemyBase, bDeathHandled);
}

float ATWEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority() || Health <= 0.0f || bDeathHandled) return 0.0f;

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ActualDamage = FMath::Min(Health, ActualDamage);

	Health -= ActualDamage;

	if (Health <= 0.0f)
	{
		OnEnemyDeath();
	}
	else
	{
		OnHealthChanged(Health, ActualDamage);
	}

	return ActualDamage;
}

void ATWEnemyBase::InitEnemy(const UEnemyDataAsset* Data, USplineComponent* Path, int32 PlayerIndex)
{
	if (!HasAuthority()) return;

	PathSpline = Path;
	TargetPlayerIndex = PlayerIndex;
	DistanceAlongSpline = 0.f;

	if (Data)
	{
		float CurrentHealthMultiplier = 1.0f;
		if (ATWGameState* GS = GetWorld()->GetGameState<ATWGameState>())
		{
			CurrentHealthMultiplier = GS->GetEnemyHealthMultiplier();
		}
		MaxHealth = Data->Health * CurrentHealthMultiplier;

		Health = MaxHealth;
		Speed = Data->Speed * 100;
		Cost = Data->Cost;
		Income = Data->Income;

		EnemyData = Data;
	}

	BaseLateralOffset = FMath::FRandRange(-MaxLateralOffset, MaxLateralOffset);

	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		ServerSpawnTime = GS->GetServerWorldTimeSeconds();
	}
	RecalculateLocationFromNetwork();
}

