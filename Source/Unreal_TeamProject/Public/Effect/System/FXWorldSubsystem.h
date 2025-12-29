#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "Effect/Core/DataType/FXType.h"
#include "Effect/Core/DataType/FXWorldType.h"
#include "Effect/Core/Interface/IFXService.h"
#include "FXWorldSubsystem.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USceneComponent;
class AActor;
class UFXRegistrySubsystem;
class UFXExecutorBase;
class UFXData;

UCLASS()
class UNREAL_TEAMPROJECT_API UFXWorldSubsystem 
	: public UTickableWorldSubsystem
	, public IFXService
{
	GENERATED_BODY()
	
public :
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float Delta) override;

public :
	virtual TStatId GetStatId() const override;

// ==== System ====
private :
	void ValidateExecutors();
	// Drain all Pending list by Budget
	void DrainPending();
	void PoolSetup();
	UNiagaraComponent* Acquire(UNiagaraSystem* FX);
	void Release(UNiagaraComponent* FXComponent);

// ==== Fire CUe ====
public :
	void FireCueOnce_AtLocation(const FGameplayTag& Tag, const FFXCueParams_World& Param);
	void FireCueOnce_Attached(const FGameplayTag& Tag, const FFXCueParams_Attached& Param);
	void FireCueLoop_Attached(const FGameplayTag& Tag, const FFXCueParams_Attached& Param);

// ==== Spawn FX ====
public :
	virtual UNiagaraComponent* SpawnOnceLocation(UNiagaraSystem* FX, const FTransform& Transform, float Scale) override;
	virtual UNiagaraComponent* SpawnOnceAttached
	(
		UNiagaraSystem* FX,
		const FGameplayTag& Tag,
		USceneComponent* Target,
		FName SocketName, 
		const FTransform& Offset,
		float Scale, 
		UActorComponent* Owner = nullptr
	) override;
	virtual UNiagaraComponent* SpawnLoopAttached
	(
		UNiagaraSystem* FX,
		const FGameplayTag& Tag,
		USceneComponent* Target,
		FName SocketName,
		const FTransform& Offset,
		float Scale,
		bool& bReused,
		bool& bReplaced,
		UActorComponent* Owner = nullptr
	) override;
	virtual void ReleaseAfter(UNiagaraComponent* FXComponent, float Time) override;
	
// ==== Socket FX ==== 
public :
	void StopAllByOwner(UActorComponent* Owner, EFXStopMode Mode = EFXStopMode::Default);
	void StopAllOnTarget(USceneComponent* Target, EFXStopMode Mode = EFXStopMode::Default);
	void StopAllOnSocket(USceneComponent* Target, FName SocketName, EFXStopMode Mode = EFXStopMode::Default);
	void StopByTagOnSocket
	(
		const FGameplayTag& Tag,
		USceneComponent* Target, 
		FName SocketName, 
		EFXStopMode Mode = EFXStopMode::Default
	);
	void StopAfterByTagOnSocket
	(
		const FGameplayTag& Tag, 
		USceneComponent* Target, 
		FName SocketName, 
		float Time, 
		EFXStopMode Mode = EFXStopMode::Default
	);
	void SetOffsetByTagOnSocket
	(
		const FGameplayTag& Tag, 
		USceneComponent* Target,
		FName SocketName, 
		const FTransform& Offset, 
		float ScaleOverride = 1.f
	);

	bool HasLoopOnSocket(const FGameplayTag& Tag, USceneComponent* Target, FName SocketName) const;
	bool HasAnyOnSocket(const FGameplayTag& Tag, USceneComponent* Target, FName SocketName) const;

// ==== Soft Stop ====
public :
	void SoftRelease(UNiagaraComponent* FXComponent);
	void SoftReleaseAfter(UNiagaraComponent* FXComponent, float Time);

// ==== FX Preparation ====
private :
	void Prepare_AtLocation(UNiagaraComponent* FXComponent, const FTransform& Transform, float Scale);
	void Prepare_Attached
	(
		UNiagaraComponent* FXComponent, 
		USceneComponent* Target, 
		FName SocketName,
		const FTransform& Offset,
		float Scale
	);

// ==== Indexing Attached ====
private :
	void RegisterFX
	(
		UNiagaraComponent* FXComponent,
		const FGameplayTag& Tag,
		USceneComponent* Target,
		FName SocketName,
		EFXType Type,
		UActorComponent* Owner = nullptr
	);
	void UnregisterFX(UNiagaraComponent* FXComponent);
	void CompactIndex();

	void ReleaseAllOnPerSocket(TMap<FName, TArray<FFXActive>>& PerSocket, EFXStopMode Mode = EFXStopMode::Default);
	void ReleaseBucket(TArray<FFXActive>& Bucket, EFXStopMode Mode = EFXStopMode::Default);

// ==== Util ====
private :
	void ReleaseAuto(UNiagaraComponent* FXComponent, EFXStopMode Mode = EFXStopMode::Default);
	void ReleaseAfterAuto(UNiagaraComponent* FXComponent, float Time, EFXStopMode Mode = EFXStopMode::Default);
	UFXExecutorBase* GetExecutor(EFXSpawnType Type);
	void CancelReleaseTimer(UNiagaraComponent* FXComponent);
	void ScheduleStopTimer(UNiagaraComponent* FXComponent, float Time, EFXStopMode Mode = EFXStopMode::Default);

// ==== Delegate ====
private :
	UFUNCTION()
	void OnNiagaraFinished(UNiagaraComponent* Finished);

	UFUNCTION()
	void OnFXActorDestroyed(AActor* DeadActor);

// ==== Base System ====
private :
	// Collection of Executor Instances
	UPROPERTY(Transient)
	TMap<EFXSpawnType, TObjectPtr<UFXExecutorBase>> Executors{};

	// Registry Subsystem for FX
	UPROPERTY()
	TObjectPtr<UFXRegistrySubsystem> FXRegistry{ nullptr };

// ==== Pending Cue ====
private:
	// Pending cue array for FX - World
	TArray<FFXPendingCue_World> FXPendings_World_Once{};

	// Pending cue array for FX - Attached
	TArray<FFXPendingCue_Attached> FXPendings_Attached_Once{};
	TArray<FFXPendingCue_Attached> FXPendings_Attached_Loop{};

// ==== Pool ====
private :
	// Pool configuration container for FX
	UPROPERTY(Transient)
	TMap<UNiagaraSystem*, FFXPoolConfig> FXPool_Config{};

	// Pool container for FX
	UPROPERTY()
	TMap<UNiagaraSystem*, FFXPool> FXPool{};

	bool bPoolConfigured{ false };

// ==== Runtime Caching ====
private :
	// For seaching Active Socket FX
	TMap<TWeakObjectPtr<USceneComponent>, TMap<FName, TArray<FFXActive>>> MetaByTarget{};

	// For searching meta for FX by component
	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<UNiagaraComponent>, FFXActive> MetaByComponent{};

	// Timer for Releasing FX
	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<UNiagaraComponent>, FTimerHandle> ReleaseTimerByComponent{};

	UPROPERTY(Transient)
	TSet<TWeakObjectPtr<AActor>> DestroyedBoundActors;

// ==== Optimization ====
private :
	// Remain budget for drain pending per frame
	int32 BudgetRemain_World_Once{};
	int32 BudgetRemain_Attached_Once{};
	int32 BudgetRemain_Attached_Loop{};

	// Execute CompactIndex evecy CompactFrame
	int32 CompactFrame{ COMPACTFRAME_DEFAULT };
	int32 TickFrameCounter{};

	// Execute when this flag is Dirty
	bool bCompactDirty{ false };
};