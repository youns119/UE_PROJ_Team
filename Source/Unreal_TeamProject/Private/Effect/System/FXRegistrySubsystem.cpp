#include "Effect/System/FXRegistrySubsystem.h"
#include "Effect/System/FXAssetManager.h"
#include "Effect/Data/FXData.h"

static UFXAssetManager* Get_FXAM_Safe()
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		return Cast<UFXAssetManager>(AssetManager);
	}

	return nullptr;
}

void UFXRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FXDataMap.Empty();
	AssetIds.Empty();
	bReady = false;

	if (UFXAssetManager* AssetManager = Get_FXAM_Safe())
	{
		AssetManager->GetAllFXDataIds(AssetIds);

		if (AssetIds.Num() == 0)
		{
			UE_LOG(LogFXRegistry, Warning, TEXT("[%s] : No FXData assets found"), 
				*GetName());
			bReady = true;

			return;
		}

		LoadFXDataHandle = AssetManager->RequestAsyncLoadFXData
		(
			AssetIds,
			FStreamableDelegate::CreateUObject(this, &UFXRegistrySubsystem::OnAllFXDataLoaded),
			FStreamableManager::AsyncLoadHighPriority
		);

		UE_LOG(LogFXRegistry, Log, TEXT("[%s] : Async preloading %d FXData assets..."),
			*GetName(), AssetIds.Num());
	}
	else
	{
		UE_LOG(LogFXRegistry, Warning, TEXT("[%s] : AssetManager invalid at Initialize"), 
			*GetName());

		bReady = true;
		return;
	}
}

void UFXRegistrySubsystem::Deinitialize()
{
	if (LoadFXDataHandle.IsValid())
	{
		LoadFXDataHandle->CancelHandle();
		LoadFXDataHandle = nullptr;
	}

	if (LoadEffectHandle.IsValid())
	{
		LoadEffectHandle->CancelHandle();
		LoadEffectHandle = nullptr;
	}

	FXDataMap.Empty();
	AssetIds.Empty();
	bReady = false;

	Super::Deinitialize();
}

const UFXData* UFXRegistrySubsystem::FindFXDataLoaded(const FGameplayTag& Tag) const
{
	if (const TObjectPtr<UFXData>* FXDataPath = FXDataMap.Find(Tag))
	{
		return FXDataPath->Get();
	}

	return nullptr;
}

void UFXRegistrySubsystem::GetAllTags(TArray<FGameplayTag>& Out) const
{
	Out.Reset(FXDataMap.Num());
	FXDataMap.GetKeys(Out);
}

void UFXRegistrySubsystem::OnAllFXDataLoaded()
{
	if (!IsValid(this) || !GetWorld())
	{
		return;
	}

	int32 DataAdded{};

	if (UFXAssetManager* AssetManager = Get_FXAM_Safe())
	{
		for (const FPrimaryAssetId& AssetId : AssetIds)
		{
			const FSoftObjectPath AssetPath = AssetManager->GetFXDataPath(AssetId);
			UFXData* FXData = TSoftObjectPtr<UFXData>(AssetPath).Get();

			if (!FXData)
			{
				UE_LOG(LogFXRegistry, Warning, TEXT("[%s] : Failed To Get FXData : %s"),
					*GetName(), *AssetPath.ToString());
				continue;
			}

			const FGameplayTag& Tag = FXData->GetCueTag();
			if (!Tag.IsValid())
			{
				UE_LOG(LogFXRegistry, Warning, TEXT("[%s] : FXData %s has invalid CueTag"),
					*GetName(), *FXData->GetName());
				continue;
			}

			if (FXDataMap.Contains(Tag))
			{
				UE_LOG(LogFXRegistry, Warning, TEXT("[%s] : Duplicate CueTag : %s (ignore %s)"),
					*GetName(), *Tag.ToString(), *FXData->GetName());
				continue;
			}

			FXDataMap.Add(Tag, FXData);
			++DataAdded;
		}

		TArray<FSoftObjectPath> EffectPaths{};
		EffectPaths.Reserve(FXDataMap.Num());
		
		for (const auto& Pair : FXDataMap)
		{
			const UFXData* FXData = Pair.Value.Get();
			const FSoftObjectPath DataPath = FXData ? FXData->GetEffectPath() : FSoftObjectPath();

			if (DataPath.IsValid())
			{
				EffectPaths.Add(DataPath);
			}
		}

		if (EffectPaths.Num() == 0)
		{
			UE_LOG(LogFXRegistry, Log, TEXT("[%s] : Ready. Loaded %d FXData (no Effects to preload)"), 
				*GetName(), DataAdded);

			bReady = true;
			return;
		}

		LoadEffectHandle = AssetManager->GetStreamableManager().RequestAsyncLoad
		(
			EffectPaths,
			FStreamableDelegate::CreateUObject(this, &UFXRegistrySubsystem::OnAllEffectsLoaded)
		);

		UE_LOG(LogFXRegistry, Log, TEXT("[%s] : Async preloading %d NiagaraSystem assets..."),
			*GetName(), DataAdded);
	}
	else
	{
		bReady = true;

		UE_LOG(LogFXRegistry, Log, TEXT("[%s] : AssetManager invalid in callback"), 
			*GetName());
	}
}

void UFXRegistrySubsystem::OnAllEffectsLoaded()
{
	if (!IsValid(this) || !GetWorld())
	{
		return;
	}

	UE_LOG(LogFXRegistry, Log, TEXT("[%s] : Ready. Loaded %d FXData + Effects"), 
		*GetName(), FXDataMap.Num());

	bReady = true;
}