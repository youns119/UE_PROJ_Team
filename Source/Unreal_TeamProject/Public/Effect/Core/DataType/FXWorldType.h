#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Effect/Core/DataType/FXType.h"
#include "FXWorldType.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USceneComponent;
class UActorComponent;
class UFXData;

constexpr int32 MAXCACHE_DEFAULT = 32;
constexpr int32 COMPACTFRAME_DEFAULT = 5;
constexpr int32 BUDGET_WORLD_ONCE = 8;
constexpr int32 BUDGET_ATTACHED_ONCE = 8;
constexpr int32 BUDGET_ATTACHED_LOOP = 8;
constexpr EFXStopMode ONCESTOP_DEFAULT = EFXStopMode::Immediate;
constexpr EFXStopMode LOOPSTOP_DEFAULT = EFXStopMode::Soft;

// Internal enum for WorldSubsystem Index/Pool
UENUM()
enum class EFXType : uint8
{
	Once_Attached,
	Loop_Attached,
};

// Pool struct for each FX
USTRUCT()
struct FFXPool
{
	GENERATED_BODY()

	// Pool for free FX
	UPROPERTY()
	TArray<TObjectPtr<UNiagaraComponent>> Pool_Free{};

	// Pool for Active FX
	TArray<TWeakObjectPtr<UNiagaraComponent>> Pool_Active{};
};

// Pool Configuration for each FX
USTRUCT()
struct FFXPoolConfig
{
	GENERATED_BODY()

// ==== Cached From FXData ====

	bool bEnable{ true };
	int32 InitialCached{ 0 };
	int32 MaxCached{ MAXCACHE_DEFAULT };
};

// Pending Cues before Registry Ready - World
USTRUCT()
struct FFXPendingCue_World
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag Tag{};

	UPROPERTY()
	FFXCueParams_World Param{};
};

// Pending Cues before Registry Ready - Attached
USTRUCT()
struct FFXPendingCue_Attached
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag Tag{};

	UPROPERTY()
	FFXCueParams_Attached Param{};
};

// Socket index meta data
USTRUCT(meta = (HiddenByDefault))
struct FFXActive
{
	GENERATED_BODY()

// ==== Component ====

	TWeakObjectPtr<UNiagaraComponent> FXComponent{};
	TWeakObjectPtr<USceneComponent> Target{};

// ==== Tag / Socket ====

	FName SocketName{ NAME_None };
	FGameplayTag Tag{};

// ==== FX ====

	TWeakObjectPtr<UNiagaraSystem> FX{ nullptr };
	TWeakObjectPtr<UActorComponent> Owner{ nullptr };

// ==== Runtime ====

	double StartTime{ 0.f };
	EFXType Type{ EFXType::Once_Attached };
};