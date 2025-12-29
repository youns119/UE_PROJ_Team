#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Effect/Core/DataType/FXType.h"
#include "FXData.generated.h"

class UNiagaraSystem;

UCLASS(BlueprintType)
class UNREAL_TEAMPROJECT_API UFXData 
	: public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public :
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("FXData"), GetFName());
	}

// ==== Getters ====
public :
// ==== Default ====
	const FGameplayTag& GetCueTag() const { return CueTag; }

// ==== FXData ====
	// Get Resolved NiagaraSystem without blocking
	UNiagaraSystem* GetResolveSystem_NoBlock() const;
	FSoftObjectPath GetEffectPath() const { return Effect.ToSoftObjectPath(); }
	float GetLifeTime() const { return LifeTime; }
	float GetBaseScale() const { return Scale; }

// ==== Pooling ====
	bool IsPoolingEnabled() const { return bUsePooling; }
	int32 GetInitialPoolSize() const { return InitialPoolSize; }
	int32 GetMaxPoolSize() const { return MaxPoolSize; }
	bool ShouldPrewarmOnLoad() const { return bPrewarmOnLoad; }

// ==== Default ====
protected :
	// Tag to identify this cue
	UPROPERTY(EditAnywhere) 
	FGameplayTag CueTag{};

	// Effect to play
	UPROPERTY(EditAnywhere) 
	TSoftObjectPtr<UNiagaraSystem> Effect{ nullptr };

	// Effect LifeTime (= 0 : default, > 0 : override)
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float LifeTime{ 1.f };

	// Effect Scale
	UPROPERTY(EditAnywhere)
	float Scale{ 1.f };

// ==== Pooling ====
protected :
	// whether to use Custom Pooling
	UPROPERTY(EditAnywhere, Category = "Pooling")
	bool bUsePooling{ false };

	// Initial Pool Size
	UPROPERTY(EditAnywhere, Category = "Pooling", meta = (ClampMin = "0", UIMin = "0"))
	int32 InitialPoolSize{ 0 };

	// Max Pool Size to prevent oversized caching
	UPROPERTY(EditAnywhere, Category = "Pooling", meta = (ClampMin = "0", UIMin = "0"))
	int32 MaxPoolSize{ 32 };

	// Whether to prewarm on load
	UPROPERTY(EditAnywhere, Category = "Pooling")
	bool bPrewarmOnLoad{ true };
};