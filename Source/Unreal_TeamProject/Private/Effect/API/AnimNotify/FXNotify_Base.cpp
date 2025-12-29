#include "Effect/API/AnimNotify/FXNotify_Base.h"
#include "Effect/API/FXSpawnerComponent.h"
#include "GameFramework/Character.h"

AActor* UFXNotify_Base::GetResolveOwner(USkeletalMeshComponent* Mesh)
{
	if (!Mesh)
	{
		UE_LOG(LogFX, Warning, TEXT("[%s] : No mesh for this AnimNofity"), *GetName());
		return nullptr;
	}

	AActor* Owner = Mesh->GetOwner();
	if (!Owner)
	{
		UE_LOG(LogFX, Warning, TEXT("[%s] : No owner actor for this AnimNofity"), *GetName());
		return nullptr;
	}

	return Owner;
}

UFXSpawnerComponent* UFXNotify_Base::GetResolveSpawner(AActor* Owner)
{
	if (!Owner)
	{
		UE_LOG(LogFX, Warning, TEXT("[%s] : No owner actor for this AnimNofity"), *GetName());
		return nullptr;
	}

	UFXSpawnerComponent* Spawner =
		SpawnerOverride ? SpawnerOverride.Get() : Owner->FindComponentByClass<UFXSpawnerComponent>();
	if (!Spawner)
	{
		UE_LOG(LogFX, Warning, TEXT("[%s] : No FXSpawnerComponent for this AnimNofity"), *GetName());
		return nullptr;
	}

	return Spawner;
}