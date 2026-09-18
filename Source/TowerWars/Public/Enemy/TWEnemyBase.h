// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TWEnemyBase.generated.h"

class USphereComponent;
class USplineComponent;
class UEnemyDataAsset;
class ATWPlayerState;
class UWidgetComponent;
class UEnemyOverheadWidget;

UCLASS()
class TOWERWARS_API ATWEnemyBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ATWEnemyBase();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void InitEnemy(const UEnemyDataAsset* Data, USplineComponent* Path, int32 InTargetPlayerIndex);
	void SetSummonerPlayerState(ATWPlayerState* InSummoner);

	FORCEINLINE float GetDistanceAlongSpline() const { return DistanceAlongSpline; }
	FORCEINLINE bool IsAlive() const { return Health > 0.f; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE int32 GetTargetPlayerIndex() const { return TargetPlayerIndex; }
	FORCEINLINE ATWPlayerState* GetSummonerPlayerState() const { return SummonerPlayerState.Get(); }

protected:
	virtual void BeginPlay() override;

	virtual void HandleReachedGoal();
	virtual void OnEnemyDeath();
	void RefreshOverheadWidget();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TW|Enemy")
	TObjectPtr<USphereComponent> HitCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TW|Enemy")
	TObjectPtr<USkeletalMeshComponent> EnemyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TW|UI")
	TObjectPtr<UWidgetComponent> OverheadWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "TW|UI")
	TSubclassOf<UEnemyOverheadWidget> OverheadWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "TW|Enemy")
	float MaxLateralOffset = 100.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "TW|Enemy")
	TObjectPtr<ATWPlayerState> SummonerPlayerState;

	// 偷取心的数量
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TW|Enemy")
	int32 HeartValue = 1;

private:	
	UPROPERTY(ReplicatedUsing = OnRep_PathSpline)
	TObjectPtr<USplineComponent> PathSpline;

	UPROPERTY(Replicated)
	int32 TargetPlayerIndex = -1;

	UPROPERTY(Replicated)
	int32 OwnerPlayerIndex = -1;

	UPROPERTY(Replicated)
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health;

	UPROPERTY(ReplicatedUsing = OnRep_Speed)
	float Speed;

	int32 Cost;
	int32 Income;
	float DistanceAlongSpline;

	UPROPERTY(Replicated)
	float BaseLateralOffset = 0.0f;

	UPROPERTY(Replicated)
	float ServerSpawnTime = 0.0f;

	UFUNCTION()
	void OnRep_PathSpline();

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	UFUNCTION()
	void OnRep_Speed();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowDeathReward(int32 GoldReward, int32 IncomeReward);

	void RecalculateLocationFromNetwork();
	void OnHealthChanged(float NewHealth, float DamageAmount);

	TWeakObjectPtr<const UEnemyDataAsset> EnemyData;

	FTimerHandle DeathDestroyTimerHandle;
	void DestroyAfterDeathReward();

	UPROPERTY(Replicated)
	bool bDeathHandled = false;
};
