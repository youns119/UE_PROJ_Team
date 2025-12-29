#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "FXType.generated.h"

class USceneComponent;
class USkeletalMeshCompoonent;

// Log Category for FX
DECLARE_LOG_CATEGORY_EXTERN(LogFX, Log, All);

UENUM(BlueprintType)
enum class EFXSpawnType : uint8
{
	Once_AtLocation		UMETA(DisplayName = "Once : AtLocation"),
	Once_Attached		UMETA(DisplayName = "Once : Attached"),
	Loop_Attached		UMETA(DisplayName = "Loop : Attached"),
	Decal_AtLocation	UMETA(DisplayName = "Decal : AtLocation"),
	CameraEffect		UMETA(DisplayName = "Camera"),
};

UENUM(BlueprintType)
enum class EFXStopMode : uint8
{
	Default		UMETA(DisplayName = "Default"),
	Immediate	UMETA(DisplayName = "Immediate"),
	Soft		UMETA(DisplayName = "Soft"),
};

UENUM(BlueprintType)
enum class EFXWorldPlacement : uint8
{
	Owner		UMETA(DisplayName = "Owner"),
	Component	UMETA(DisplayName = "Component"),
	Socket		UMETA(DisplayName = "Socket"),
	Custom		UMETA(DisplayName = "Custom"),
	Spawner		UMETA(DisplayName = "Spawner"),
};

USTRUCT(BlueprintType)
struct FFXWorldPlacement
{
	GENERATED_BODY()

	// Placement Type of World FX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|World|Placement")
	EFXWorldPlacement PlaceType{ EFXWorldPlacement::Owner };

	// Component to attach to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|World|Placement",
		meta = (EditCondition = "PlaceType == EFXWorldPlacement::Component || PlaceType == EFXWorldPlacement::Socket"))
	FComponentReference ComponentRef{};

	// Name of the Socket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|World|Placement",
		meta = (EditCondition = "PlaceType == EFXWorldPlacement::Socket"))
	FName SocketName{ NAME_None };

	// Custom Transform
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|World|Placement",
		meta = (EditCondition = "PlaceType == EFXWorldPlacement::Custom"))
	FTransform CustomTransform = { FTransform::Identity };

	// Offset to the current transform
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|World|Placement")
	FTransform RelativeOffset{ FTransform::Identity };
};

USTRUCT(BlueprintType)
struct FFXCueParams_World
{
	GENERATED_BODY()

	// Cue Tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|World")
	FGameplayTag CueTag{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|World")
	FTransform WorldTransform{ FTransform::Identity };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|World")
	float Scale{ 1.f };

	// Override default lifetime if >= 0.f
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|World")
	float LifeTimeOverride{ -1.f };
};

USTRUCT(BlueprintType)
struct FFXAttachInfo
{
	GENERATED_BODY()

	// Component to attach to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|Attach|AttachInfo")
	FComponentReference ComponentRef{};

	// Socket / Bone Name to attach to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|Attach|AttachInfo")
	FName SocketName{ NAME_None };

	// Relative Offset Transform to Socket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|Attach|AttachInfo")
	FTransform RelativeOffset{ FTransform::Identity };
};

USTRUCT(BlueprintType)
struct FFXCueParams_Attached
{
	GENERATED_BODY()

	// Cue Tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|Attach")
	FGameplayTag CueTag{};

	// Attach Info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|Attach")
	FFXAttachInfo AttachInfo{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|Attach")
	float Scale{ 1.f };

	// For loop update
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|Attach")
	bool  bRestart{ false };

	// Override default lifetime if >= 0.f
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX|Attach")
	float LifeTimeOverride{ -1.f };

	// Component to attach to
	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> AttachTarget{ nullptr };

	// Components that owns the FX
	UPROPERTY(Transient)
	TWeakObjectPtr<UActorComponent> Owner;
};