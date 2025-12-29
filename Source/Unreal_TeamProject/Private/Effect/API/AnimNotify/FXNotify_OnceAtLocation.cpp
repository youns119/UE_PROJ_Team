#include "Effect/API/AnimNotify/FXNotify_OnceAtLocation.h"
#include "Effect/API/FXSpawnerComponent.h"
#include "Effect/Util/FXUtil.h"

void UFXNotify_OnceAtLocation::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Anim)
{
	if (AActor* Owner = GetResolveOwner(Mesh))
	{
		if (UFXSpawnerComponent* Spawner = GetResolveSpawner(Owner))
		{
			Spawner->SpawnOnceLocation(Tag, WorldPlacement, ScaleOverride, LifeTimeOverride);
		}
	}
}