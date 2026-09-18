// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/TWTowerBase.h"
#include "Enemy/TWEnemyBase.h"
#include "Data/TowerDataAsset.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "UI/TowerOverheadWidget.h"

ATWTowerBase::ATWTowerBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetNetUpdateFrequency(10.0f);

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	TowerMesh->SetupAttachment(RootComponent);

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidgetComponent->SetupAttachment(RootComponent);
	OverheadWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	OverheadWidgetComponent->SetDrawAtDesiredSize(true);
	OverheadWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
}

void ATWTowerBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (OverheadWidgetComponent && OverheadWidgetClass)
	{
		OverheadWidgetComponent->SetWidgetClass(OverheadWidgetClass);

		if (UTowerOverheadWidget* Widget = Cast<UTowerOverheadWidget>(OverheadWidgetComponent->GetUserWidgetObject()))
		{
			Widget->InitTower(this);
		}
	}
}

void ATWTowerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(CurrentTarget)
	{
		FVector TargetLoc = CurrentTarget->GetActorLocation();
		FVector MeshLoc = TowerMesh->GetComponentLocation();
		FRotator TargetRot = (TargetLoc - MeshLoc).Rotation();
		FRotator NewRot = FRotator(0.0f, TargetRot.Yaw, 0.0f);
		TowerMesh->SetWorldRotation(FMath::RInterpTo(TowerMesh->GetComponentRotation(), NewRot, DeltaTime, 10.0f));
	}
}

void ATWTowerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATWTowerBase, CurrentTarget);
	DOREPLIFETIME(ATWTowerBase, OwnerPlayerIndex);
	DOREPLIFETIME(ATWTowerBase, TowerData);
}

void ATWTowerBase::InitTower(UTowerDataAsset* Data, int32 PlayerIndex)
{
	if (!HasAuthority() || !Data) return;

	TowerData = Data;
	OwnerPlayerIndex = PlayerIndex;

	Damage = Data->Damage;
	AttackRange = Data->AttackRange * 100.0f;
	AttackInterval = Data->AttackInterval;
	SplashRadius = Data->SplashRadius * 100.0f;
	DamageType = Data->DamageType;
	TargetStrategy = Data->TargetStrategy;

	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ATWTowerBase::CheckAndAttack, AttackInterval, true);

	RefreshOverheadWidget();
}

void ATWTowerBase::CheckAndAttack()
{
	if (!HasAuthority()) return;

	TArray<ATWEnemyBase*> ValidTargets;
	AcquireTargets(ValidTargets);
	if (ValidTargets.Num() == 0)
	{
		CurrentTarget = nullptr;
		return;
	}

	ATWEnemyBase* PrimaryTarget = SelectBestTarget(ValidTargets);
	CurrentTarget = PrimaryTarget;
	if (!PrimaryTarget) return;

	TArray<ATWEnemyBase*> FinalHitTargets;
	if (DamageType == ETowerDamageType::SingleTarget)
	{
		FinalHitTargets.Add(PrimaryTarget);
	}
	else if (DamageType == ETowerDamageType::AreaOfEffect)
	{
		FVector Origin = PrimaryTarget->GetActorLocation();
		for (ATWEnemyBase* Candidate : ValidTargets)
		{
			if (Candidate && FVector::DistSquared(Origin, Candidate->GetActorLocation()) <= FMath::Square(SplashRadius))
			{
				FinalHitTargets.Add(Candidate);
			}
		}
	}

	PerformAttack(FinalHitTargets);
	Multicast_PlayAttackEffects(PrimaryTarget, FinalHitTargets);
}

ATWEnemyBase* ATWTowerBase::SelectBestTarget(const TArray<ATWEnemyBase*>& CandidateEnemies)
{
	if (CandidateEnemies.Num() == 0) return nullptr;

	ATWEnemyBase* BestEnemy = CandidateEnemies[0];
	for (int32 i = 1; i < CandidateEnemies.Num(); ++i)
	{
		ATWEnemyBase* Curr = CandidateEnemies[i];
		if (!Curr->IsAlive()) continue;

		switch (TargetStrategy)
		{
		case ETowerTargetStrategy::First:
			// 离终点最近：DistanceAlongSpline 越大越优先
			if (Curr->GetDistanceAlongSpline() > BestEnemy->GetDistanceAlongSpline())
			{
				BestEnemy = Curr;
			}
			break;

		case ETowerTargetStrategy::Last:
			// 离起点最近：DistanceAlongSpline 越小越优先
			if (Curr->GetDistanceAlongSpline() < BestEnemy->GetDistanceAlongSpline())
			{
				BestEnemy = Curr;
			}
			break;

		case ETowerTargetStrategy::Closest:
			// 离塔最近
			if (GetDistanceTo(Curr) < GetDistanceTo(BestEnemy))
			{
				BestEnemy = Curr;
			}
			break;

		case ETowerTargetStrategy::Strongest:
			// 血量最高
			if (Curr->GetHealth() > BestEnemy->GetHealth())
			{
				BestEnemy = Curr;
			}
			break;

		case ETowerTargetStrategy::Weakest:
			// 残血优先
			if (Curr->GetHealth() < BestEnemy->GetHealth())
			{
				BestEnemy = Curr;
			}
			break;

		default:
			break;
		}
	}

	return BestEnemy;
}

void ATWTowerBase::PerformAttack(const TArray<ATWEnemyBase*>& Targets)
{
	if (!HasAuthority()) return;

	for (ATWEnemyBase* Target : Targets)
	{
		if (!Target || !Target->IsAlive()) continue;

		UGameplayStatics::ApplyDamage(
			Target,
			Damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);

		ApplySpecialEffects(Target);
	}
}

void ATWTowerBase::PlayAttackEffects_Implementation(ATWEnemyBase* PrimaryTarget, const TArray<ATWEnemyBase*>& AllHitTargets)
{
}

void ATWTowerBase::Multicast_PlayAttackEffects_Implementation(ATWEnemyBase* PrimaryTarget, const TArray<ATWEnemyBase*>& AllHitTargets)
{
	PlayAttackEffects(PrimaryTarget, AllHitTargets);
}

void ATWTowerBase::ApplySpecialEffects(ATWEnemyBase* TargetEnemy)
{
}

void ATWTowerBase::AcquireTargets(TArray<ATWEnemyBase*>& OutTargets)
{
	OutTargets.Empty();

	TArray<FOverlapResult> Overlaps;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(AttackRange);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		ECC_WorldDynamic,
		SphereShape,
		QueryParams
	);
	if (!bHit) return;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (ATWEnemyBase* Enemy = Cast<ATWEnemyBase>(Overlap.GetActor()))
		{
			if (Enemy->IsAlive())
			{
				OutTargets.AddUnique(Enemy);
			}
		}
	}
}

void ATWTowerBase::RefreshOverheadWidget()
{
	if (OverheadWidgetComponent)
	{
		if (UTowerOverheadWidget* Widget = Cast<UTowerOverheadWidget>(OverheadWidgetComponent->GetUserWidgetObject()))
		{
			Widget->Refresh();
		}
	}
}
