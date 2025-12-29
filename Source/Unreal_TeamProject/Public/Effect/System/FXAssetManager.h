#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Engine/AssetManagerTypes.h"
#include "FXAssetManager.generated.h"

// Log Category for FXRegistrySubsystem
DECLARE_LOG_CATEGORY_EXTERN(LogFXRegistry, Log, All);

UCLASS()
class UNREAL_TEAMPROJECT_API UFXAssetManager 
	: public UAssetManager
{
	GENERATED_BODY()
	
public :
	virtual void StartInitialLoading() override;

public :
	// Get the current AssetManager object
	static UFXAssetManager& Get()
	{
		UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
		check(AssetManager);

		return *CastChecked<UFXAssetManager>(AssetManager);
	}

	// Get All FXData Primary Asset Ids
	void GetAllFXDataIds(TArray<FPrimaryAssetId>& Out) const
	{
		GetPrimaryAssetIdList(FXDataType, Out);
	}

	// Get FXData Primary Asset Path from Primary Asset Id
	FSoftObjectPath GetFXDataPath(const FPrimaryAssetId& AssetId) const
	{
		return GetPrimaryAssetPath(AssetId);
	}

	void GetAllFXDataPaths(TArray<FSoftObjectPath>& Out) const;

public :
	// Request Loading of FXData Assets Async (For Pre-warming)
	TSharedPtr<FStreamableHandle> RequestAsyncLoadFXData
	(
		const TArray<FPrimaryAssetId>& AssetIds,
		FStreamableDelegate OnLoaded,
		int32 Priority = FStreamableManager::AsyncLoadHighPriority
	) const;

private :
	// Primary Asset Type for FXData
	static const FPrimaryAssetType FXDataType;
};