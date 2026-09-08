// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Type/TWType.h"
#include "TWTowerBase.generated.h"

class UTowerDataAsset;
class ATWEnemyBase;

UCLASS()
class TOWERWARS_API ATWTowerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ATWTowerBase();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitTower(UTowerDataAsset* Data, int32 PlayerIndex);

	FORCEINLINE int32 GetOwnerPlayerIndex() const { return OwnerPlayerIndex; }
	FORCEINLINE UTowerDataAsset* GetTowerData() const { return TowerData; }

	UPROPERTY(VisibleAnywhere, Category = "TW|Tower")
	FIntPoint OccupiedGridCoord;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "TW|Components")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, Category = "TW|Components")
	TObjectPtr<UStaticMeshComponent> TowerMesh;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = "TW|Debug")
	TObjectPtr<ATWEnemyBase> CurrentTarget;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = "TW|Debug")
	TObjectPtr<UTowerDataAsset> TowerData;

	virtual void CheckAndAttack();
	virtual void AcquireTargets(TArray<ATWEnemyBase*>& OutTargets);
	virtual ATWEnemyBase* SelectBestTarget(const TArray<ATWEnemyBase*>& CandidateEnemies);
	virtual void PerformAttack(const TArray<ATWEnemyBase*>& Targets);
	virtual void ApplySpecialEffects(ATWEnemyBase* TargetEnemy);

	UFUNCTION(BlueprintNativeEvent, Category = "TW|Tower")
	void PlayAttackEffects(ATWEnemyBase* PrimaryTarget, const TArray<ATWEnemyBase*>& AllHitTargets);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayAttackEffects(ATWEnemyBase* PrimaryTarget, const TArray<ATWEnemyBase*>& AllHitTargets);

private:
	UPROPERTY(Replicated)
	int32 OwnerPlayerIndex = -1;

	float Damage;
	float AttackRange;
	float AttackInterval;
	float SplashRadius;
	ETowerDamageType DamageType;
	ETowerTargetStrategy TargetStrategy;

	FTimerHandle AttackTimerHandle;
};
