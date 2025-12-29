#include "Effect/System/FXWorldSubsystem.h"
#include "Effect/System/FXRegistrySubsystem.h"
#include "Effect/Executor/FXExec_OnceLocation.h"
#include "Effect/Executor/FXExec_OnceAttached.h"
#include "Effect/Executor/FXExec_LoopAttached.h"
#include "Effect/Data/FXData.h"
#include "Effect/Util/FXUtil.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Stats/Stats.h"

DEFINE_LOG_CATEGORY(LogFX);

void UFXWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Executors.Empty();
	FXPendings_World_Once.Empty();
	FXPendings_Attached_Once.Empty();
	FXPendings_Attached_Loop.Empty();

	FXPool.Empty();
	FXPool_Config.Empty();
	bPoolConfigured = false;

	MetaByTarget.Empty();
	MetaByComponent.Empty();
	ReleaseTimerByComponent.Empty();
	DestroyedBoundActors.Empty();

	FXRegistry = nullptr;
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			FXRegistry = GameInstance->GetSubsystem<UFXRegistrySubsystem>();
		}
		else
		{
			UE_LOG(LogFX, Warning, TEXT("[%s] : No GameInstance Exist"), *GetName());
			return;
		}
	}
	else
	{
		UE_LOG(LogFX, Warning, TEXT("[%s] : No World Exist"), *GetName());
		return;
	}

	ValidateExecutors();

	TickFrameCounter = 0;
	bCompactDirty = false;

	BudgetRemain_World_Once = 0;
	BudgetRemain_Attached_Once = 0;
	BudgetRemain_Attached_Loop = 0;
}

void UFXWorldSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		for (auto& TimerHandle : ReleaseTimerByComponent)
		{
			World->GetTimerManager().ClearTimer(TimerHandle.Value);
		}
	}

	ReleaseTimerByComponent.Empty();

	for (auto it = DestroyedBoundActors.CreateIterator(); it; ++it)
	{
		if (AActor* Actor = it->Get())
		{
			Actor->OnDestroyed.RemoveDynamic(this, &UFXWorldSubsystem::OnFXActorDestroyed);
		}
	}

	DestroyedBoundActors.Empty();

	for (auto& Pair : FXPool)
	{
		for (UNiagaraComponent* FXComponent_Free : Pair.Value.Pool_Free)
		{
			if (IsValid(FXComponent_Free))
			{
				FXComponent_Free->DestroyComponent();
			}
		}

		for (TWeakObjectPtr<UNiagaraComponent> FXComponent_Weak : Pair.Value.Pool_Active)
		{
			if (UNiagaraComponent* FXComponent_Active = FXComponent_Weak.Get())
			{
				FXComponent_Active->DeactivateImmediate();
				FXComponent_Active->DestroyComponent();
			}
		}
	}

	FXPool.Empty();
	FXPool_Config.Empty();

	MetaByTarget.Empty();
	MetaByComponent.Empty();

	Executors.Empty();
	FXPendings_World_Once.Empty();
	FXPendings_Attached_Once.Empty();
	FXPendings_Attached_Loop.Empty();

	FXRegistry = nullptr;

	Super::Deinitialize();
}

void UFXWorldSubsystem::Tick(float Delta)
{
	bool RegistryReady = FXRegistry && FXRegistry->IsReady();

	if (!bPoolConfigured && RegistryReady)
	{
		PoolSetup();
		bPoolConfigured = true;
	}

	if (RegistryReady)
	{
		BudgetRemain_World_Once = BUDGET_WORLD_ONCE;
		BudgetRemain_Attached_Once = BUDGET_ATTACHED_ONCE;
		BudgetRemain_Attached_Loop = BUDGET_ATTACHED_LOOP;

		DrainPending();
	}

	TickFrameCounter++;
	const bool bFrameGate = (CompactFrame <= 1) || (TickFrameCounter % CompactFrame == 0);

	if (bCompactDirty && bFrameGate)
	{
		CompactIndex();
		bCompactDirty = false;
	}
}

TStatId UFXWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFXWorldSubsystem, STATGROUP_Tickables);
}

void UFXWorldSubsystem::ValidateExecutors()
{
	if (!Executors.Contains(EFXSpawnType::Once_AtLocation))
	{
		Executors.Add(EFXSpawnType::Once_AtLocation, NewObject<UFXExec_OnceLocation>(this));
	}
	if (!Executors.Contains(EFXSpawnType::Once_Attached))
	{
		Executors.Add(EFXSpawnType::Once_Attached, NewObject<UFXExec_OnceAttached>(this));
	}
	if (!Executors.Contains(EFXSpawnType::Loop_Attached))
	{
		Executors.Add(EFXSpawnType::Loop_Attached, NewObject<UFXExec_LoopAttached>(this));
	}
}

void UFXWorldSubsystem::DrainPending()
{
	if (!FXRegistry || !FXRegistry->IsReady())
	{
		return;
	}

	while (BudgetRemain_World_Once > 0 && FXPendings_World_Once.Num() > 0)
	{
		FFXPendingCue_World PendingCue = FXPendings_World_Once[0];
		FXPendings_World_Once.RemoveAtSwap(0, 1, false);

		const UFXData* Data = FXRegistry->FindFXDataLoaded(PendingCue.Tag);
		if (!Data)
		{
			UE_LOG(LogFX, Warning, TEXT("[%s] Pending_World_Once drop : unknown Tag %s"),
				*GetName(), *PendingCue.Tag.ToString());

			continue;
		}

		if (UFXExecutorBase* Exec = GetExecutor(EFXSpawnType::Once_AtLocation))
		{
			Exec->Execute_World(*this, Data, PendingCue.Param);
			--BudgetRemain_World_Once;
		}
	}

	while (BudgetRemain_Attached_Once > 0 && FXPendings_Attached_Once.Num() > 0)
	{
		FFXPendingCue_Attached PendingCue = FXPendings_Attached_Once[0];
		FXPendings_Attached_Once.RemoveAtSwap(0, 1, false);

		const UFXData* Data = FXRegistry->FindFXDataLoaded(PendingCue.Tag);
		if (!Data)
		{
			UE_LOG(LogFX, Warning, TEXT("[%s] Pending_Attached_Once drop : unknown Tag %s"),
				*GetName(), *PendingCue.Tag.ToString());

			continue;;
		}

		if (UFXExecutorBase* Exec = GetExecutor(EFXSpawnType::Once_Attached))
		{
			Exec->Execute_Attached(*this, Data, PendingCue.Param);
			--BudgetRemain_Attached_Once;
		}
	}

	while (BudgetRemain_Attached_Loop > 0 && FXPendings_Attached_Loop.Num() > 0)
	{
		FFXPendingCue_Attached PendingCue = FXPendings_Attached_Loop[0];
		FXPendings_Attached_Loop.RemoveAtSwap(0, 1, false);

		const UFXData* Data = FXRegistry->FindFXDataLoaded(PendingCue.Tag);
		if (!Data)
		{
			UE_LOG(LogFX, Warning, TEXT("[%s] Pending_Attached_Loop drop : unknown Tag %s"),
				*GetName(), *PendingCue.Tag.ToString());

			continue;;
		}

		if (UFXExecutorBase* Exec = GetExecutor(EFXSpawnType::Loop_Attached))
		{
			Exec->Execute_Attached(*this, Data, PendingCue.Param);
			--BudgetRemain_Attached_Loop;
		}
	}
}

void UFXWorldSubsystem::PoolSetup()
{
	if (!FXRegistry || !FXRegistry->IsReady())
	{
		UE_LOG(LogFX, Warning, TEXT("[%s] : FXRegistry not ready"), *GetName());
		return;
	}

	TArray<FGameplayTag> Tags{};
	FXRegistry->GetAllTags(Tags);

	for (const FGameplayTag& Tag : Tags)
	{
		const UFXData* Data = FXRegistry->FindFXDataLoaded(Tag);
		if (!Data)
		{
			UE_LOG(LogFX, Warning, TEXT("[%s] : FXData not loaded (%s)"),
				*GetName(), *Tag.ToString());

			continue;;
		}

		UNiagaraSystem* FX = Data->GetResolveSystem_NoBlock();
		if (!FX)
		{
			UE_LOG(LogFX, Warning, TEXT("[%s] : FX not loaded (%s)"),
				*GetName(), *Tag.ToString());

			continue;
		}

		FFXPoolConfig PoolConfig{};
		PoolConfig.bEnable = Data->IsPoolingEnabled();
		PoolConfig.InitialCached = FMath::Max(0, Data->GetInitialPoolSize());
		PoolConfig.MaxCached = FMath::Max(0, Data->GetMaxPoolSize());
		FXPool_Config.FindOrAdd(FX) = PoolConfig;

		if (!PoolConfig.bEnable)
		{
			continue;
		}

		FFXPool& Pool = FXPool.FindOrAdd(FX);
		Pool.Pool_Free.Reserve(PoolConfig.InitialCached);

		if (Data->ShouldPrewarmOnLoad() && PoolConfig.InitialCached > 0)
		{
			int32 FXNum = PoolConfig.InitialCached - Pool.Pool_Free.Num();
			for (int32 i = 0; i < FXNum; ++i)
			{
				UNiagaraComponent* FXComponent = Acquire(FX);
				Release(FXComponent);
			}
		}
	}
}

UNiagaraComponent* UFXWorldSubsystem::Acquire(UNiagaraSystem* FX)
{
	FFXPool& Pool = FXPool.FindOrAdd(FX);

	while (Pool.Pool_Free.Num() > 0)
	{
		UNiagaraComponent* FXComponent = Pool.Pool_Free.Pop(false);

		if (!IsValid(FXComponent))
		{
			if (FXComponent)
			{
				FXComponent->DestroyComponent();
			}

			continue;
		}

		CancelReleaseTimer(FXComponent);
		Pool.Pool_Active.Add(FXComponent);

		return FXComponent;
	}

	UNiagaraComponent* FXComponent = NewObject<UNiagaraComponent>(GetWorld());
	FXComponent->SetAsset(FX);
	FXComponent->bAutoActivate = false;
	FXComponent->SetAutoDestroy(false);
	FXComponent->OnSystemFinished.AddDynamic(this, &UFXWorldSubsystem::OnNiagaraFinished);
	FXComponent->RegisterComponentWithWorld(GetWorld());
	FXComponent->SetCanEverAffectNavigation(false);

	Pool.Pool_Active.Add(FXComponent);

	return FXComponent;
}

void UFXWorldSubsystem::Release(UNiagaraComponent* FXComponent)
{
	if (!IsValid(FXComponent))
	{
		return;
	}

	CancelReleaseTimer(FXComponent);

	UNiagaraSystem* FX = FXComponent->GetAsset();
	if (!FX)
	{
		if (const FFXActive* Meta = MetaByComponent.Find(FXComponent))
		{
			FX = Meta->FX.Get();
		}
	}

	UnregisterFX(FXComponent);

	FXComponent->DeactivateImmediate();
	FXComponent->SetVisibility(false, true);
	FXComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	if (!FX)
	{
		FXComponent->DestroyComponent();
		bCompactDirty = true;

		return;
	}

	FFXPool* Pool = FXPool.Find(FX);
	if (Pool)
	{
		Pool->Pool_Active.Remove(FXComponent);
	}

	const FFXPoolConfig* PoolConfig = FXPool_Config.Find(FX);
	const bool bPoolEnabled = PoolConfig ? PoolConfig->bEnable : true;
	const int32 MaxCache = PoolConfig ? PoolConfig->MaxCached : MAXCACHE_DEFAULT;

	if (bPoolEnabled)
	{
		int32 FreeCnt = Pool ? Pool->Pool_Free.Num() : 0;

		if (FreeCnt < MaxCache)
		{
			if (!Pool)
			{
				Pool = &FXPool.Add(FX, FFXPool{});
			}

			if (!Pool->Pool_Free.Contains(FXComponent))
			{
				Pool->Pool_Free.Add(FXComponent);
			}

			bCompactDirty = true;
			return;
		}
	}

	FXComponent->DestroyComponent();
	bCompactDirty = true;
}

void UFXWorldSubsystem::FireCueOnce_AtLocation(const FGameplayTag& Tag, const FFXCueParams_World& Param)
{
	if (BudgetRemain_World_Once <= 0 || !FXRegistry || !FXRegistry->IsReady())
	{
		FXPendings_World_Once.Emplace(FFXPendingCue_World{ Tag, Param });
		return;
	}

	const UFXData* Data = FXRegistry->FindFXDataLoaded(Tag);
	if (!Data)
	{
		FXPendings_World_Once.Emplace(FFXPendingCue_World{ Tag, Param });
		return;
	}

	if (UFXExecutorBase* Exec = GetExecutor(EFXSpawnType::Once_AtLocation))
	{
		Exec->Execute_World(*this, Data, Param);
		--BudgetRemain_World_Once;
	}
	else
	{
		FXPendings_World_Once.Emplace(FFXPendingCue_World{ Tag, Param });
	}
}

void UFXWorldSubsystem::FireCueOnce_Attached(const FGameplayTag& Tag, const FFXCueParams_Attached& Param)
{
	if (BudgetRemain_Attached_Once <= 0 || !FXRegistry || !FXRegistry->IsReady())
	{
		FXPendings_Attached_Once.Emplace(FFXPendingCue_Attached{ Tag, Param });
		return;
	}

	const UFXData* Data = FXRegistry->FindFXDataLoaded(Tag);
	if (!Data)
	{
		FXPendings_Attached_Once.Emplace(FFXPendingCue_Attached{ Tag, Param });
		return;
	}

	if (UFXExecutorBase* Exec = GetExecutor(EFXSpawnType::Once_Attached))
	{
		Exec->Execute_Attached(*this, Data, Param);
		--BudgetRemain_Attached_Once;
	}
	else
	{
		FXPendings_Attached_Once.Emplace(FFXPendingCue_Attached{ Tag, Param });
	}
}

void UFXWorldSubsystem::FireCueLoop_Attached(const FGameplayTag& Tag, const FFXCueParams_Attached& Param)
{
	if (BudgetRemain_Attached_Loop <= 0 || !FXRegistry || !FXRegistry->IsReady())
	{
		FXPendings_Attached_Loop.Emplace(FFXPendingCue_Attached{ Tag, Param });
		return;
	}

	const UFXData* Data = FXRegistry->FindFXDataLoaded(Tag);
	if (!Data)
	{
		FXPendings_Attached_Loop.Emplace(FFXPendingCue_Attached{ Tag, Param });
		return;
	}

	if (UFXExecutorBase* Exec = GetExecutor(EFXSpawnType::Loop_Attached))
	{
		Exec->Execute_Attached(*this, Data, Param);
		--BudgetRemain_Attached_Loop;
	}
	else
	{
		FXPendings_Attached_Loop.Emplace(FFXPendingCue_Attached{ Tag, Param });
	}
}

UNiagaraComponent* UFXWorldSubsystem::SpawnOnceLocation(UNiagaraSystem* FX, const FTransform& Transform, float Scale)
{
	if (!FX || !GetWorld())
	{
		return nullptr;
	}

	UNiagaraComponent* FXComponent = Acquire(FX);
	if (!IsValid(FXComponent))
	{
		UE_LOG(LogFX, Warning, TEXT("[%s] : Acquire failed (OnceLocation): %s"), *GetName(), *GetNameSafe(FX));
		return nullptr;
	}

	Prepare_AtLocation(FXComponent, Transform, Scale);
	FXComponent->ActivateSystem(true);

	return FXComponent;
}

UNiagaraComponent* UFXWorldSubsystem::SpawnOnceAttached
(
	UNiagaraSystem* FX, 
	const FGameplayTag& Tag,
	USceneComponent* Target, 
	FName SocketName, 
	const FTransform& Offset,
	float Scale, 
	UActorComponent* Owner
)
{
	if (!FX || !IsValid(Target))
	{
		return nullptr;
	}

	UNiagaraComponent* FXComponent = Acquire(FX);
	if (!IsValid(FXComponent))
	{
		UE_LOG(LogFX, Warning, TEXT("[%s] : Acquire failed (OnceAttached): %s"), *GetName(), *GetNameSafe(FX));
		return nullptr;
	}

	if (!IsValid(Target))
	{
		Release(FXComponent);

		return nullptr;
	}

	Prepare_Attached(FXComponent, Target, SocketName, Offset, Scale);
	FXComponent->Activate(true);

	RegisterFX(FXComponent, Tag, Target, SocketName, EFXType::Once_Attached, Owner);

	return FXComponent;
}

UNiagaraComponent* UFXWorldSubsystem::SpawnLoopAttached
(
	UNiagaraSystem* FX,
	const FGameplayTag& Tag, 
	USceneComponent* Target, 
	FName SocketName, 
	const FTransform& Offset,
	float Scale,
	bool& bReused, 
	bool& bReplaced,
	UActorComponent* Owner
)
{
	bReused = false; 
	bReplaced = false;

	if (!FX || !IsValid(Target))
	{
		return nullptr;
	}

	TMap<FName, TArray<FFXActive>>& PerSocket = MetaByTarget.FindOrAdd(Target);
	TArray<FFXActive>& Bucket = PerSocket.FindOrAdd(SocketName);
	Bucket.RemoveAll([](const FFXActive& Meta)
		{ 
			return !Meta.FXComponent.IsValid();
		});

	int32 PrimaryIndex = INDEX_NONE;
	TArray<TWeakObjectPtr<UNiagaraComponent>> Duplicates;
	Duplicates.Reserve(Bucket.Num());

	for (int32 i = 0; i < Bucket.Num(); ++i)
	{
		const FFXActive& Meta = Bucket[i];
		if (Meta.FXComponent.IsValid() && Meta.Type == EFXType::Loop_Attached && Meta.Tag == Tag)
		{
			if (PrimaryIndex == INDEX_NONE)
			{
				PrimaryIndex = i;
			}
			else
			{
				Duplicates.Add(Meta.FXComponent);
			}
		}
	}

	for (const auto& DupComponent : Duplicates)
	{
		if (UNiagaraComponent* Dup = DupComponent.Get())
		{
			SoftRelease(Dup);
		}
	}

	if (Duplicates.Num() > 0)
	{
		Bucket.RemoveAll([](const FFXActive& Meta) 
			{ 
				return !Meta.FXComponent.IsValid();
			});
	}

	if (PrimaryIndex != INDEX_NONE)
	{
		if (UNiagaraComponent* FXComponent = Bucket[PrimaryIndex].FXComponent.Get())
		{
			if (FXComponent->GetAsset() == FX) 
			{ 
				bReused = true; 
				return FXComponent; 
			}

			bReplaced = true; 
			Release(FXComponent);
		}
	}

	UNiagaraComponent* FXComponent = Acquire(FX);
	if (!IsValid(FXComponent))
	{
		UE_LOG(LogFX, Warning, TEXT("[%s] : Acquire failed (LoopAttached): %s"), *GetName(), *GetNameSafe(FX));
		return nullptr;
	}

	if (!IsValid(Target))
	{
		Release(FXComponent);

		return nullptr;
	}

	Prepare_Attached(FXComponent, Target, SocketName, Offset, Scale);
	FXComponent->ActivateSystem(true);

	RegisterFX(FXComponent, Tag, Target, SocketName, EFXType::Loop_Attached, Owner);

	return FXComponent;
}

void UFXWorldSubsystem::ReleaseAfter(UNiagaraComponent* FXComponent, float Time)
{
	ScheduleStopTimer(FXComponent, Time, EFXStopMode::Immediate);
}

void UFXWorldSubsystem::StopAllByOwner(UActorComponent* Owner, EFXStopMode Mode)
{
	if (!IsValid(Owner))
	{
		return;
	}

	TArray<TWeakObjectPtr<UNiagaraComponent>> FXComponentsToStop;
	FXComponentsToStop.Reserve(MetaByComponent.Num());

	for (const auto& MetaPair : MetaByComponent)
	{
		const TWeakObjectPtr<UNiagaraComponent> FXComponent = MetaPair.Key;
		const FFXActive& Meta = MetaPair.Value;

		if (Meta.Owner == Owner && FXComponent.IsValid())
		{
			FXComponentsToStop.Add(FXComponent);
		}
	}

	for (const TWeakObjectPtr<UNiagaraComponent>& FXComponent_Weak : FXComponentsToStop)
	{
		if (UNiagaraComponent* FXComponent = FXComponent_Weak.Get())
		{
			ReleaseAuto(FXComponent, Mode);
		}
	}

	bCompactDirty = true;
}

void UFXWorldSubsystem::StopAllOnTarget(USceneComponent* Target, EFXStopMode Mode)
{
	if (!IsValid(Target))
	{
		return;
	}

	if (TMap<FName, TArray<FFXActive>>* PerSocket = MetaByTarget.Find(Target))
	{
		ReleaseAllOnPerSocket(*PerSocket, Mode);
		MetaByTarget.Remove(Target);

		bCompactDirty = true;
	}
}

void UFXWorldSubsystem::StopAllOnSocket(USceneComponent* Target, FName SocketName, EFXStopMode Mode)
{
	if (!IsValid(Target))
	{
		return;
	}

	TArray<TWeakObjectPtr<UNiagaraComponent>> FXComponentsToStop;
	if (TMap<FName, TArray<FFXActive>>* PerSocket = MetaByTarget.Find(Target))
	{
		if (TArray<FFXActive>* Bucket = PerSocket->Find(SocketName))
		{
			FXComponentsToStop.Reserve(Bucket->Num());

			for (const FFXActive& Meta : *Bucket)
			{
				if (Meta.FXComponent.IsValid())
				{
					FXComponentsToStop.Add(Meta.FXComponent);
				}
			}
		}
	}

	for (const TWeakObjectPtr<UNiagaraComponent>& FXComponent_Weak : FXComponentsToStop)
	{
		if (UNiagaraComponent* FXComponent = FXComponent_Weak.Get())
		{
			ReleaseAuto(FXComponent, Mode);
		}
	}

	bCompactDirty = true;
}

void UFXWorldSubsystem::StopByTagOnSocket
(
	const FGameplayTag& Tag,
	USceneComponent* Target, 
	FName SocketName, 
	EFXStopMode Mode
)
{
	if (!IsValid(Target))
	{
		return;
	}

	TArray<TWeakObjectPtr<UNiagaraComponent>> FXComponentsToStop;
	if (TMap<FName, TArray<FFXActive>>* PerSocket = MetaByTarget.Find(Target))
	{
		if (TArray<FFXActive>* Bucket = PerSocket->Find(SocketName))
		{
			for (const FFXActive& Meta : *Bucket)
			{
				if (!Meta.FXComponent.IsValid())
				{
					continue;
				}

				if (Tag.IsValid() && Meta.Tag != Tag)
				{
					continue;
				}

				FXComponentsToStop.Add(Meta.FXComponent);
			}
		}
	}

	for (const TWeakObjectPtr<UNiagaraComponent>& FXComponent_Weak : FXComponentsToStop)
	{
		if (UNiagaraComponent* FXComponent = FXComponent_Weak.Get())
		{
			ReleaseAuto(FXComponent, Mode);
		}
	}

	bCompactDirty = true;
}

void UFXWorldSubsystem::StopAfterByTagOnSocket
(
	const FGameplayTag& Tag, 
	USceneComponent* Target, 
	FName SocketName, 
	float Time, 
	EFXStopMode Mode
)
{
	if (!IsValid(Target))
	{
		return;
	}

	TArray<TWeakObjectPtr<UNiagaraComponent>> FXComponentsToStop;
	if (TMap<FName, TArray<FFXActive>>* PerSocket = MetaByTarget.Find(Target))
	{
		if (TArray<FFXActive>* Bucket = PerSocket->Find(SocketName))
		{
			for (const FFXActive& Meta : *Bucket)
			{
				if (!Meta.FXComponent.IsValid())
				{
					continue;
				}

				if (Tag.IsValid() && Meta.Tag != Tag)
				{
					continue;
				}

				FXComponentsToStop.Add(Meta.FXComponent);
			}
		}
	}

	for (const TWeakObjectPtr<UNiagaraComponent>& FXComponent_Weak : FXComponentsToStop)
	{
		if (UNiagaraComponent* FXComponent = FXComponent_Weak.Get())
		{
			ReleaseAfterAuto(FXComponent, Time, Mode);
		}
	}

	bCompactDirty = true;
}

void UFXWorldSubsystem::SetOffsetByTagOnSocket
(
	const FGameplayTag& Tag, 
	USceneComponent* Target, 
	FName SocketName, 
	const FTransform& Offset, 
	float ScaleOverride
)
{
	if (!IsValid(Target))
	{
		return;
	}

	if (TMap<FName, TArray<FFXActive>>* PerSocket = MetaByTarget.Find(Target))
	{
		if (TArray<FFXActive>* Bucket = PerSocket->Find(SocketName))
		{
			for (FFXActive& Meta : *Bucket)
			{
				if (!Meta.FXComponent.IsValid())
				{
					continue;
				}

				if (Tag.IsValid() && Meta.Tag != Tag)
				{
					continue;
				}

				if (UNiagaraComponent* FXComponent = Meta.FXComponent.Get())
				{
					FXUtil::ApplyRelativeOffset(FXComponent, Offset, ScaleOverride);
				}
			}
		}
	}
}

bool UFXWorldSubsystem::HasLoopOnSocket(const FGameplayTag& Tag, USceneComponent* Target, FName SocketName) const
{
	if (!IsValid(Target))
	{
		return false;
	}

	if (const TMap<FName, TArray<FFXActive>>* PerSocket = MetaByTarget.Find(Target))
	{
		if (const TArray<FFXActive>* Bucket = PerSocket->Find(SocketName))
		{
			for (const FFXActive& Meta : *Bucket)
			{
				if (Meta.FXComponent.IsValid() && Meta.Type == EFXType::Loop_Attached && Meta.Tag == Tag)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool UFXWorldSubsystem::HasAnyOnSocket(const FGameplayTag& Tag, USceneComponent* Target, FName SocketName) const
{
	if (!IsValid(Target))
	{
		return false;
	}

	if (const TMap<FName, TArray<FFXActive>>* PerSocket = MetaByTarget.Find(Target))
	{
		if (const TArray<FFXActive>* Bucket = PerSocket->Find(SocketName))
		{
			for (const FFXActive& Meta : *Bucket)
			{
				if (Meta.FXComponent.IsValid() && (!Tag.IsValid() || Meta.Tag == Tag))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void UFXWorldSubsystem::SoftRelease(UNiagaraComponent* FXComponent)
{
	if (!IsValid(FXComponent))
	{
		return;
	}

	const FFXActive* Meta = MetaByComponent.Find(FXComponent);
	if (!Meta)
	{
		Release(FXComponent);
		return;
	}

	CancelReleaseTimer(FXComponent);
	if (FXComponent->IsActive())
	{
		FXComponent->Deactivate();
	}
}

void UFXWorldSubsystem::SoftReleaseAfter(UNiagaraComponent* FXComponent, float Time)
{
	ScheduleStopTimer(FXComponent, Time, EFXStopMode::Soft);
}

void UFXWorldSubsystem::Prepare_AtLocation(UNiagaraComponent* FXComponent, const FTransform& Transform, float Scale)
{
	if (!FXComponent)
	{
		return;
	}

	FXComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	FXComponent->SetWorldTransform(Transform);
	FXComponent->SetWorldScale3D(FVector(Scale));

	FXComponent->SetVisibility(true, true);
}

void UFXWorldSubsystem::Prepare_Attached
(
	UNiagaraComponent* FXComponent,
	USceneComponent* Target,
	FName SocketName,
	const FTransform& Offset,
	float Scale
)
{
	if (!FXComponent || !IsValid(Target))
	{
		return;
	}

	FXComponent->AttachToComponent(Target, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

	const FTransform TransformTR(Offset.GetRotation(), Offset.GetLocation(), FVector::OneVector);
	FXComponent->SetRelativeTransform(TransformTR, false, nullptr, ETeleportType::TeleportPhysics);
	FXComponent->SetRelativeScale3D(FVector(Scale));

	FXComponent->SetVisibility(true, true);
}

void UFXWorldSubsystem::RegisterFX
(
	UNiagaraComponent* FXComponent, 
	const FGameplayTag& Tag, 
	USceneComponent* Target, 
	FName SocketName, 
	EFXType Type,
	UActorComponent* Owner
)
{
	if (!IsValid(FXComponent) || !IsValid(Target))
	{
		return;
	}

	FFXActive Meta{};
	Meta.FXComponent = FXComponent;
	Meta.Target = Target;
	Meta.Tag = Tag;
	Meta.SocketName = SocketName;
	Meta.StartTime = FPlatformTime::Seconds();
	Meta.Type = Type;
	Meta.FX = FXComponent->GetAsset();
	Meta.Owner = Owner;

	TMap<FName, TArray<FFXActive>>& PerSocket = MetaByTarget.FindOrAdd(Target);
	TArray<FFXActive>& Bucket = PerSocket.FindOrAdd(SocketName);

	Bucket.Add(Meta);
	MetaByComponent.Add(FXComponent, Meta);

	if (AActor* OwnerActor = Target->GetOwner())
	{
		if (!DestroyedBoundActors.Contains(OwnerActor))
		{
			OwnerActor->OnDestroyed.AddUniqueDynamic(this, &UFXWorldSubsystem::OnFXActorDestroyed);
			DestroyedBoundActors.Add(OwnerActor);
		}
	}

	bCompactDirty = true;
}

void UFXWorldSubsystem::UnregisterFX(UNiagaraComponent* FXComponent)
{
	if (!FXComponent)
	{
		return;
	}

	if (FFXActive* Meta = MetaByComponent.Find(FXComponent))
	{
		const TWeakObjectPtr<USceneComponent> Target = Meta->Target;
		const FName SocketName = Meta->SocketName;

		if (TMap<FName, TArray<FFXActive>>* PerSocket = MetaByTarget.Find(Target))
		{
			if (TArray<FFXActive>* Bucket = PerSocket->Find(SocketName))
			{
				Bucket->RemoveAll([FXComponent](const FFXActive& M)
					{
						return M.FXComponent.Get() == FXComponent;
					});

				if (Bucket->Num() == 0)
				{
					PerSocket->Remove(SocketName);
				}
			}

			if (PerSocket->Num() == 0)
			{
				MetaByTarget.Remove(Target);
			}
		}
	}

	MetaByComponent.Remove(FXComponent);

	bCompactDirty = true;
}

void UFXWorldSubsystem::CompactIndex()
{
	for (auto it = MetaByTarget.CreateIterator(); it; ++it)
	{
		if (!it->Key.IsValid())
		{
			ReleaseAllOnPerSocket(it->Value);
			it.RemoveCurrent();
		}
	}

	for (auto& MetaPair : MetaByTarget)
	{
		TMap<FName, TArray<FFXActive>>& PerSocket = MetaPair.Value;

		for (auto& SocketPair : PerSocket)
		{
			TArray<FFXActive>& Bucket = SocketPair.Value;

			Bucket.RemoveAll([](const FFXActive& Meta)
				{
					return !Meta.FXComponent.IsValid();
				});
		}
	}

	for (auto it = MetaByComponent.CreateIterator(); it; ++it)
	{
		if (!it->Key.IsValid())
		{
			it.RemoveCurrent();
		}
	}

	if (UWorld* World = GetWorld())
	{
		for (auto it = ReleaseTimerByComponent.CreateIterator(); it; ++it)
		{
			if (!it->Key.IsValid())
			{
				World->GetTimerManager().ClearTimer(it->Value);
				it.RemoveCurrent();
			}
		}
	}

	for (auto& Pair : FXPool)
	{
		Pair.Value.Pool_Active.RemoveAll([](const TWeakObjectPtr<UNiagaraComponent>& FXComponent) 
			{ 
				return !FXComponent.IsValid();
			});

		Pair.Value.Pool_Free.RemoveAll([](const TObjectPtr<UNiagaraComponent> FXComponent) 
			{ 
				return !IsValid(FXComponent);
			});
	}

	bCompactDirty = false;
}

void UFXWorldSubsystem::ReleaseAllOnPerSocket(TMap<FName, TArray<FFXActive>>& PerSocket, EFXStopMode Mode)
{
	for (auto& SocketPair : PerSocket)
	{
		TArray<FFXActive>& Bucket = SocketPair.Value;
		ReleaseBucket(Bucket, Mode);
	}

	PerSocket.Reset();
}

void UFXWorldSubsystem::ReleaseBucket(TArray<FFXActive>& Bucket, EFXStopMode Mode)
{
	TArray<TWeakObjectPtr<UNiagaraComponent>> FXComponentsToRelease{};
	FXComponentsToRelease.Reserve(Bucket.Num());

	for (const FFXActive& Meta : Bucket)
	{
		if (UNiagaraComponent* FXComponent = Meta.FXComponent.Get())
		{
			FXComponentsToRelease.Add(FXComponent);
		}
	}

	for (const auto& FXComponent_Weak : FXComponentsToRelease)
	{
		if (UNiagaraComponent* FXComponent = FXComponent_Weak.Get())
		{
			ReleaseAuto(FXComponent, Mode);
		}
	}

	Bucket.Reset();
}

void UFXWorldSubsystem::ReleaseAuto(UNiagaraComponent* FXComponent, EFXStopMode Mode)
{
	if (!IsValid(FXComponent))
	{
		return;
	}

	if (const FFXActive* Meta = MetaByComponent.Find(FXComponent))
	{
		if (Mode == EFXStopMode::Default)
		{
			EFXStopMode StopMode{};

			if (Meta->Type == EFXType::Once_Attached)
			{
				StopMode = ONCESTOP_DEFAULT;
			}
			else
			{
				StopMode = LOOPSTOP_DEFAULT;
			}

			if (StopMode == EFXStopMode::Immediate)
			{
				Release(FXComponent);
			}
			else
			{
				SoftRelease(FXComponent);
			}
		}
		else
		{
			if (Mode == EFXStopMode::Immediate)
			{
				Release(FXComponent);
			}
			else
			{
				SoftRelease(FXComponent);
			}
		}
	}
	else
	{
		Release(FXComponent);
	}
}

void UFXWorldSubsystem::ReleaseAfterAuto(UNiagaraComponent* FXComponent, float Time, EFXStopMode Mode)
{
	if (!IsValid(FXComponent))
	{
		return;
	}

	if (const FFXActive* Meta = MetaByComponent.Find(FXComponent))
	{
		if (Mode == EFXStopMode::Default)
		{
			EFXStopMode StopMode{};

			if (Meta->Type == EFXType::Once_Attached)
			{
				StopMode = ONCESTOP_DEFAULT;
			}
			else
			{
				StopMode = LOOPSTOP_DEFAULT;
			}

			if (StopMode == EFXStopMode::Immediate)
			{
				ReleaseAfter(FXComponent, Time);
			}
			else
			{
				SoftReleaseAfter(FXComponent, Time);
			}
		}
		else
		{
			if (Mode == EFXStopMode::Immediate)
			{
				ReleaseAfter(FXComponent, Time);
			}
			else
			{
				SoftReleaseAfter(FXComponent, Time);
			}
		}
	}
	else
	{
		ReleaseAfter(FXComponent, Time);
	}
}

UFXExecutorBase* UFXWorldSubsystem::GetExecutor(EFXSpawnType Type)
{
	if (TObjectPtr<UFXExecutorBase>* Executor = Executors.Find(Type))
	{
		return Executor->Get();
	}

	return nullptr;
}

void UFXWorldSubsystem::CancelReleaseTimer(UNiagaraComponent* FXComponent)
{
	if (!IsValid(FXComponent) || !GetWorld())
	{
		return;
	}

	if (FTimerHandle* Handle = ReleaseTimerByComponent.Find(FXComponent))
	{
		GetWorld()->GetTimerManager().ClearTimer(*Handle);
		ReleaseTimerByComponent.Remove(FXComponent);
	}

	bCompactDirty = true;
}

void UFXWorldSubsystem::ScheduleStopTimer(UNiagaraComponent* FXComponent, float Time, EFXStopMode Mode)
{
	if (!IsValid(FXComponent) || !GetWorld() || Time <= 0.f)
	{
		return;
	}

	CancelReleaseTimer(FXComponent);

	TWeakObjectPtr<UFXWorldSubsystem> Self_Weak(this);
	TWeakObjectPtr<UNiagaraComponent> FXComponent_Weak(FXComponent);
	FTimerHandle Handle{};

	if (Mode == EFXStopMode::Soft)
	{
		GetWorld()->GetTimerManager().SetTimer(
			Handle,
			[Self_Weak, FXComponent_Weak]()
			{
				if (UFXWorldSubsystem* WorldSubsystem = Self_Weak.Get())
				{
					if (UNiagaraComponent* Component = FXComponent_Weak.Get())
					{
						WorldSubsystem->SoftRelease(Component);
					}
				}
			},
			Time, false);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(
			Handle,
			[Self_Weak, FXComponent_Weak]()
			{
				if (UFXWorldSubsystem* WorldSubsystem = Self_Weak.Get())
				{
					if (UNiagaraComponent* Component = FXComponent_Weak.Get())
					{
						WorldSubsystem->Release(Component);
					}
				}
			},
			Time, false);
	}

	ReleaseTimerByComponent.FindOrAdd(FXComponent, Handle);

	bCompactDirty = true;
}

void UFXWorldSubsystem::OnNiagaraFinished(UNiagaraComponent* Finished)
{
	static TSet<UNiagaraComponent*> DupDelegate;
	if (DupDelegate.Contains(Finished))
	{
		return;
	}

	DupDelegate.Add(Finished);
	Release(Finished);
	DupDelegate.Remove(Finished);

	bCompactDirty = true;
}

void UFXWorldSubsystem::OnFXActorDestroyed(AActor* DeadActor)
{
	if (!DeadActor)
	{
		return;
	}

	if (DestroyedBoundActors.Contains(DeadActor))
	{
		DeadActor->OnDestroyed.RemoveDynamic(this, &UFXWorldSubsystem::OnFXActorDestroyed);
		DestroyedBoundActors.Remove(DeadActor);
	}

	TArray<TWeakObjectPtr<USceneComponent>> TargetsToClear;
	TargetsToClear.Reserve(MetaByTarget.Num());
	for (const auto& PerSocket : MetaByTarget)
	{
		if (USceneComponent* Target = PerSocket.Key.Get())
		{
			if (Target->GetOwner() == DeadActor)
			{
				TargetsToClear.Add(PerSocket.Key);
			}
		}
	}

	TArray<TWeakObjectPtr<UNiagaraComponent>> FXComponentsToStop;
	for (const TWeakObjectPtr<USceneComponent>& Target_Weak : TargetsToClear)
	{
		if (USceneComponent* Target = Target_Weak.Get())
		{
			if (TMap<FName, TArray<FFXActive>>* PerSocket = MetaByTarget.Find(Target))
			{
				for (auto& SocketPair : *PerSocket)
				{
					for (FFXActive& Meta : SocketPair.Value)
					{
						if (UNiagaraComponent* FXComponent = Meta.FXComponent.Get())
						{
							FXComponentsToStop.Add(Meta.FXComponent);
						}
					}
				}
			}
		}
	}

	for (const TWeakObjectPtr<UNiagaraComponent>& FXComponent_Weak : FXComponentsToStop)
	{
		if (UNiagaraComponent* FXComponent = FXComponent_Weak.Get())
		{
			CancelReleaseTimer(FXComponent);

			if (const FFXActive* Meta = MetaByComponent.Find(FXComponent))
			{
				if (Meta->Type == EFXType::Loop_Attached) 
				{ 
					SoftRelease(FXComponent);
				}
				else 
				{ 
					Release(FXComponent); 
				}
			}
			else
			{
				Release(FXComponent);
			}
		}
	}

	for (auto it = MetaByComponent.CreateIterator(); it; ++it)
	{
		if (!it->Key.IsValid())
		{
			it.RemoveCurrent();
		}
	}

	if (UWorld* World = GetWorld())
	{
		for (auto it = ReleaseTimerByComponent.CreateIterator(); it; ++it)
		{
			if (!it->Key.IsValid())
			{
				World->GetTimerManager().ClearTimer(it->Value);
				it.RemoveCurrent();
			}
		}
	}

	bCompactDirty = true;
}