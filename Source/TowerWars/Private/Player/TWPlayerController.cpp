// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TWPlayerController.h"
#include "Grid/TWGridSubsystem.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "UI/TWHUD.h"
#include "UI/TWOverlay.h"
#include "Player/TWPlayerState.h"
#include "Data/TowerDataAsset.h"
#include "Data/EnemyDataAsset.h"
#include "Tower/TWTowerBase.h"
#include "Game/TWGameMode.h"

void ATWPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

    UTWGridSubsystem* GridSubsystem = GetWorld()->GetSubsystem<UTWGridSubsystem>();
    if (!GridSubsystem) return;

	ATWPlayerState* PS = GetPlayerState<ATWPlayerState>();
	const int32 LocalPlayerId =	PS ? PS->GetPlayerIndex() : -1;
	if (LocalPlayerId < 0) return;

    // 2. 鼠标射线检测地面
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

    if (HitResult.bBlockingHit)
    {
        // 3. 换算为网格坐标，并获取该格子的中心点坐标
        FIntPoint HoveredCoord = GridSubsystem->WorldToGridCoords(HitResult.ImpactPoint);
        FVector TileCenter = GridSubsystem->GridToWorldLocation(HoveredCoord);

		const FGridCell* Cell = GridSubsystem->GetCellData(HoveredCoord);
		FColor BoxColor = FColor::Red;
		if (Cell)
		{
			if (Cell->OwnerPlayerId != LocalPlayerId)
			{
				// 别人的区域
				BoxColor = FColor::Red;
			}
			else
			{
				switch (Cell->CellType)
				{
				case EGridCellType::Buildable:
					// 可以建塔
					BoxColor = FColor::Green;
					break;

				case EGridCellType::Built:
					// 已有塔
					BoxColor = FColor::Yellow;
					break;

				case EGridCellType::Blocked:
					// 禁止建造
					BoxColor = FColor::Red;
					break;
				default:
					BoxColor = FColor::Red;
					break;
				}
			}
		}
		DrawDebugBox(GetWorld(), TileCenter, FVector(50.f, 50.f, 2.f), BoxColor, false, -1.f, 0, 3.f);
    }
}

void ATWPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	NotifyPlayerStateReady();
}

void ATWPlayerController::InitPlayerState()
{
	Super::InitPlayerState();

	NotifyPlayerStateReady();
}

void ATWPlayerController::RequestBuildTower(UTowerDataAsset* TowerData)
{
	if (!TowerData || !TowerData->TowerClass) return;

	Server_RequestBuildTower(SelectedGridCoord, TowerData);
}

void ATWPlayerController::RequestUpgradeTower(UTowerDataAsset* TargetTowerData)
{
	Server_RequestUpgradeTower(SelectedGridCoord, TargetTowerData);
}

void ATWPlayerController::RequestSellTower()
{
	Server_RequestSellTower(SelectedGridCoord);
}

void ATWPlayerController::RequestSummonEnemy(UEnemyDataAsset* EnemyData)
{
	Server_RequestSummonEnemy(EnemyData);
}

void ATWPlayerController::BeginPlay()
{
    Super::BeginPlay();

	HUD = Cast<ATWHUD>(GetHUD());

	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); 
	InputMode.SetHideCursorDuringCapture(false);                      
	SetInputMode(InputMode);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATWPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &ThisClass::OnLeftClick);
        EnhancedInputComponent->BindAction(SummonAction, ETriggerEvent::Started, this, &ThisClass::OnSummonClick);
    }

}

void ATWPlayerController::OnLeftClick()
{
    UTWGridSubsystem* GridSubsystem = GetWorld()->GetSubsystem<UTWGridSubsystem>();
    if (!GridSubsystem) return;

    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

    if(HitResult.bBlockingHit)
    {
		FIntPoint ClickedCoord = GridSubsystem->WorldToGridCoords(HitResult.ImpactPoint);
        const FGridCell* Cell = GridSubsystem->GetCellData(ClickedCoord);

		ATWPlayerState* PS = GetPlayerState<ATWPlayerState>();
		int32 LocalPlayerId = PS ? PS->GetPlayerIndex() : -1;

        if (Cell && Cell->OwnerPlayerId == LocalPlayerId)
        {
			SelectedGridCoord = ClickedCoord;

            switch (Cell->CellType)
            {
            case EGridCellType::Buildable:
                // 1. 点击空地 -> 打开建造菜单
				HUD->ShowBuildMenu();
                break;

            case EGridCellType::Built:
                // 2. 点击已有塔 -> 打开升级/拆除菜单
                if (Cell->PlacedTower.IsValid())
                {
                    HUD->ShowUpgradeMenu(Cell->PlacedTower->GetTowerData());
                }
                break;
            }
        }
	}
}

void ATWPlayerController::OnSummonClick()
{
	HUD->ShowSummonMenu();
}

void ATWPlayerController::NotifyPlayerStateReady()
{
	if (IsLocalController() && HUD.IsValid())
	{
		if (UTWOverlay* Overlay = HUD->GetOverlayWidget())
		{
			Overlay->TryInitInfoMenu();
		}
	}

	if (ATWPlayerState* PS = GetPlayerState<ATWPlayerState>())
	{
		OnPlayerStateReady.Broadcast(PS);
	}
}

bool ATWPlayerController::Server_RequestSummonEnemy_Validate(UEnemyDataAsset* EnemyData)
{
	return true;
}

void ATWPlayerController::Server_RequestSummonEnemy_Implementation(UEnemyDataAsset* EnemyData)
{
	if (!EnemyData) return;

	ATWPlayerState* PS = GetPlayerState<ATWPlayerState>();
	ATWGameMode* GM = GetWorld()->GetAuthGameMode<ATWGameMode>();
	if (!PS || !GM) return;

	// 金币校验
	const int32 CurrentStar = PS->GetEnemyStarLevel(EnemyData);
	const int32 ActualCost = EnemyData->GetCostForStar(CurrentStar);
	const int32 ActualIncome = EnemyData->GetIncomeForStar(CurrentStar);
	if (PS->GetGold() < ActualCost) return;

	// Stock校验
	if (!PS->TryConsumeStock(EnemyData, 1)) return;

	// 扣除金币，并增加玩家的 Income（收益）
	PS->AddGold(-ActualCost);
	PS->AddIncome(ActualIncome);

	// 广播给其他所有人刷怪
	const int32 MyPlayerIndex = PS->GetPlayerIndex();
	GM->SummonEnemyToAllOthers(MyPlayerIndex, EnemyData);
}

void ATWPlayerController::Server_RequestSellTower_Implementation(FIntPoint GridCoord)
{
	UTWGridSubsystem* GridSubsystem = GetWorld()->GetSubsystem<UTWGridSubsystem>();
	if (!GridSubsystem) return;

	const FGridCell* Cell = GridSubsystem->GetCellData(GridCoord);
	ATWPlayerState* PS = GetPlayerState<ATWPlayerState>();
	if (!Cell || !PS) return;

	if (Cell->OwnerPlayerId != PS->GetPlayerIndex() || Cell->CellType != EGridCellType::Built) return;

	ATWTowerBase* Tower = Cell->PlacedTower.Get();
	if (!IsValid(Tower) || !Tower->GetTowerData()) return;

	PS->AddGold(FMath::FloorToInt(Tower->GetTowerData()->Cost / 2.f));

	Tower->Destroy();

	GridSubsystem->ReleaseCell(PS->GetPlayerIndex(), GridCoord);
}

bool ATWPlayerController::Server_RequestSellTower_Validate(FIntPoint GridCoord)
{
	return true;
}

void ATWPlayerController::Server_RequestUpgradeTower_Implementation(FIntPoint GridCoord, UTowerDataAsset* TargetTowerData)
{
	if (!HasAuthority() || !TargetTowerData) return;

	ATWPlayerState* PS = GetPlayerState<ATWPlayerState>();
	UWorld* World = GetWorld();
	if (!PS || !World) return;

	// 1. 获取网格子系统
	UTWGridSubsystem* GridSubsystem = World->GetSubsystem<UTWGridSubsystem>();
	if (!GridSubsystem) return;

	// 2. 校验网格数据与领地归属
	const FGridCell* CellData = GridSubsystem->GetCellData(GridCoord);
	if (!CellData || CellData->OwnerPlayerId != PS->GetPlayerIndex()) return;

	// 3. 从 FGridCell 中获取旧防御塔（通过 TWeakObjectPtr 的 Get() 方法）
	ATWTowerBase* ExistingTower = CellData->PlacedTower.Get();
	if (!ExistingTower) return;

	UTowerDataAsset* CurrentData = ExistingTower->GetTowerData();
	if (!CurrentData) return;

	// 4. 防篡改校验一：目标塔必须属于当前塔 NextUpgrades 数组中的合法分支
	if (!CurrentData->NextUpgrades.Contains(TargetTowerData)) return;

	// 5. 防篡改校验二：玩家收入门槛校验 (Cost / 5)
	if (!TargetTowerData->IsUnlocked(PS->GetIncome())) return;

	// 6. 防篡改校验三：计算差价并校验金币
	const int32 CostDifference = FMath::Max(0, TargetTowerData->Cost - CurrentData->Cost);
	if (PS->GetGold() < CostDifference) return;

	// 7. 扣除差价金币
	PS->AddGold(-CostDifference);

	// 8. 计算精准网格坐标与保存旧塔朝向
	const FVector OldSpawnLocation = GridSubsystem->GridToWorldLocation(GridCoord);
	const FRotator OldSpawnRotation = ExistingTower->GetActorRotation();

	// 9. 销毁旧塔并在网格中释放该单元格
	ExistingTower->Destroy();
	GridSubsystem->ReleaseCell(PS->GetPlayerIndex(), GridCoord);

	// 10. 生成新级别的防御塔 Actor
	if (TargetTowerData->TowerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetPawn();

		ATWTowerBase* NewTower = World->SpawnActor<ATWTowerBase>(
			TargetTowerData->TowerClass,
			OldSpawnLocation,
			OldSpawnRotation,
			SpawnParams
		);

		if (NewTower)
		{
			NewTower->OccupiedGridCoord = GridCoord;
			NewTower->InitTower(TargetTowerData, PS->GetPlayerIndex());

			// 11. 在网格子系统中重新占用单元格并绑定新塔
			GridSubsystem->OccupyCell(PS->GetPlayerIndex(), GridCoord, NewTower);
		}
	}
}

bool ATWPlayerController::Server_RequestUpgradeTower_Validate(FIntPoint GridCoord, UTowerDataAsset* TargetTowerData)
{
	return TargetTowerData != nullptr;
}

void ATWPlayerController::Server_RequestBuildTower_Implementation(FIntPoint GridCoord, UTowerDataAsset* TowerData)
{
	if (!TowerData || !TowerData->TowerClass)
	{
		return;
	}

	// 校验 1：GridSubsystem 状态
	UTWGridSubsystem* GridSubsystem = GetWorld()->GetSubsystem<UTWGridSubsystem>();
	if (!GridSubsystem) return;

	const FGridCell* Cell = GridSubsystem->GetCellData(GridCoord);
	ATWPlayerState* PS = GetPlayerState<ATWPlayerState>();
	if (!Cell || !PS) return;

	// 校验 2：防作弊校验（地块归属 & 是否为空）
	if (Cell->OwnerPlayerId != PS->GetPlayerIndex() || Cell->CellType != EGridCellType::Buildable)
	{
		return;
	}

	// 校验 3：玩家金币是否足够 (Hypixel 规则)
	if (PS->GetGold() < TowerData->Cost)
	{
		// 可发 RPC 通知客户端：金币不足
		return;
	}

	// --- 执行建造 ---
	// 1. 扣钱
	PS->AddGold(-TowerData->Cost);

	// 2. 获取生成世界坐标
	FVector SpawnLoc = GridSubsystem->GridToWorldLocation(GridCoord);
	FRotator SpawnRot = FRotator::ZeroRotator;

	// 3. 生成防御塔 Actor
	ATWTowerBase* NewTower = GetWorld()->SpawnActorDeferred<ATWTowerBase>(
		TowerData->TowerClass,
		FTransform(SpawnRot, SpawnLoc),
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (NewTower)
	{
		NewTower->OccupiedGridCoord = GridCoord;
		NewTower->InitTower(TowerData, PS->GetPlayerIndex());
		NewTower->FinishSpawning(FTransform(SpawnRot, SpawnLoc));

		// 4. 更新服务端 GridSubsystem 中的网格状态（记录塔指针与类型）
		GridSubsystem->OccupyCell(PS->GetPlayerIndex(), GridCoord, NewTower);
	}
}

bool ATWPlayerController::Server_RequestBuildTower_Validate(FIntPoint GridCoord, UTowerDataAsset* TowerData)
{
	return TowerData != nullptr;
}
