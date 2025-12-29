#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Effect/Core/DataType/FXType.h"
#include "FXSpawnerComponent.generated.h"

class UFXWorldSubsystem;

UCLASS( ClassGroup=(FX), meta=(BlueprintSpawnableComponent) )
class UNREAL_TEAMPROJECT_API UFXSpawnerComponent 
	: public UActorComponent
{
	GENERATED_BODY()

public :	
	UFXSpawnerComponent();

protected :
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

// ==== Spawn FX ====
public :
    // Spawn FX On Location Once
    UFUNCTION(BlueprintCallable, Category = "FX|World", 
        meta = (AutoCreateRefTerm = "PlaceType"))
    void SpawnOnceLocation
    (
        FGameplayTag Tag, 
        const FFXWorldPlacement& PlaceType,
        float ScaleOverride = -1.f,
        float LifeTimeOverride = -1.f
    );

    // Spawn FX On Socket Once
    UFUNCTION(BlueprintCallable, Category = "FX|Attached", 
        meta = (AutoCreateRefTerm = "AttachInfo"))
    void SpawnOnceAttached
    (
        FGameplayTag Tag, 
        FFXAttachInfo AttachInfo,
        float ScaleOverride = -1.f,
        float LifeTimeOverride = -1.f
    );

    // Spawn FX On Socket Loop
    UFUNCTION(BlueprintCallable, Category = "FX|Attached", 
        meta = (AutoCreateRefTerm = "AttachInfo", AdvancedDisplay = "bRestart"))
    void SpawnLoopAttached
    (
        FGameplayTag Tag, 
        FFXAttachInfo AttachInfo,
        float ScaleOverride = -1.f,
        float LifeTimeOverride = -1.f,
        bool bRestart = false
    );

// ==== Socket FX ====
public :
    // Stop All FX on the Socket
    UFUNCTION(BlueprintCallable, Category = "FX|SocketControl",
        meta = (AutoCreateRefTerm = "Target"))
    void StopAllOnSocket
    (
        const FComponentReference& Target, 
        FName SocketName, 
        EFXStopMode Mode = EFXStopMode::Default
    );

    // Stop FX by tag on the Socket
    UFUNCTION(BlueprintCallable, Category = "FX|SocketControl",
        meta = (AutoCreateRefTerm = "Target"))
    void StopByTagOnSocket
    (
        FGameplayTag Tag, 
        const FComponentReference& Target, 
        FName SocketName, 
        EFXStopMode Mode = EFXStopMode::Default
    );

    // Stop FX by tag after time on the Socket
    UFUNCTION(BlueprintCallable, Category = "FX|SocketControl",
        meta = (AutoCreateRefTerm = "Target"))
    void StopAfterByTagOnSocket
    (
        FGameplayTag Tag, 
        const FComponentReference& Target, 
        FName SocketName, 
        float Time, 
        EFXStopMode Mode = EFXStopMode::Default
    );

    // Reset offset of FX by tag on the Socket
    UFUNCTION(BlueprintCallable, Category = "FX|SocketControl", 
        meta = (AutoCreateRefTerm = "Target, Offset"))
    void SetOffsetByTagOnSocket
    (
        FGameplayTag Tag, 
        const FComponentReference& Target, 
        FName SocketName, 
        const FTransform& Offset, 
        float ScaleOverride = -1.f
    );

    // Whether the Socket has Loop FX by Tag
    UFUNCTION(BlueprintPure, Category = "FX|SocketControl",
        meta = (AutoCreateRefTerm = "Target"))
    bool HasLoopOnSocket(FGameplayTag Tag, const FComponentReference& Target, FName SocketName) const;

    // Whether the Socket has any FX by Tag
    UFUNCTION(BlueprintPure, Category = "FX|SocketControl",
        meta = (AutoCreateRefTerm = "Target"))
    bool HasAnyOnSocket(FGameplayTag Tag, const FComponentReference& Target, FName SocketName) const;

private :
    UPROPERTY(EditAnywhere, Category = "FX|Attached")
    FComponentReference DefaultTarget{};

    UPROPERTY(EditAnywhere, Category = "FX|Attached")
    FName DefaultSocketName{ NAME_None };
};