#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/AssetManager.h"
#include "GameplayTagContainer.h"
#include "FXRegistrySubsystem.generated.h"

class UFXData;

UCLASS()
class UNREAL_TEAMPROJECT_API UFXRegistrySubsystem 
	: public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public :
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public :
	// Return whether the subsystem is ready
	UFUNCTION(BlueprintPure)
	bool IsReady() const { return bReady; }

	// Return FXData if loaded
	UFUNCTION(BlueprintPure)
	const UFXData* FindFXDataLoaded(const FGameplayTag& Tag) const;

public :
	// Get all tags
	void GetAllTags(TArray<FGameplayTag>& Out) const;

private :
	// 2-Stage async Prewarming callback
	void OnAllFXDataLoaded();
	void OnAllEffectsLoaded();

private :
	// Map of GameplayTag to FXData
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UFXData>> FXDataMap{};

private :
	// Handles for async loads
	TSharedPtr<FStreamableHandle> LoadFXDataHandle{ nullptr };
	TSharedPtr<FStreamableHandle> LoadEffectHandle{ nullptr };

	// Id list for FXData loading
	TArray<FPrimaryAssetId> AssetIds{};

	// Whether the subsystem is ready
	bool bReady{ false };
};