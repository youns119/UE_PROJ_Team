#include "Effect/System/FXAssetManager.h"

DEFINE_LOG_CATEGORY(LogFXRegistry);

const FPrimaryAssetType UFXAssetManager::FXDataType = FPrimaryAssetType(TEXT("FXData"));

void UFXAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	TArray<FPrimaryAssetId> AssetIds{};
	GetAllFXDataIds(AssetIds);

	UE_LOG(LogFXRegistry, Log, TEXT("[%s] : FXDatas Found : %d"), 
		*GetName(), AssetIds.Num());
}

void UFXAssetManager::GetAllFXDataPaths(TArray<FSoftObjectPath>& Out) const
{
	TArray<FPrimaryAssetId> AssetIds{};
	GetAllFXDataIds(AssetIds);

	Out.Reset();
	Out.Reserve(AssetIds.Num());

	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		Out.Add(GetFXDataPath(AssetId));
	}
}

TSharedPtr<FStreamableHandle> UFXAssetManager::RequestAsyncLoadFXData
(
	const TArray<FPrimaryAssetId>& AssetIds, 
	FStreamableDelegate OnLoaded, 
	int32 Priority
) const
{
	TArray<FSoftObjectPath> AssetPaths{};
	AssetPaths.Reserve(AssetIds.Num());
	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		AssetPaths.Add(GetFXDataPath(AssetId));
	}

	return GetStreamableManager().RequestAsyncLoad(AssetPaths, MoveTemp(OnLoaded), Priority);
}