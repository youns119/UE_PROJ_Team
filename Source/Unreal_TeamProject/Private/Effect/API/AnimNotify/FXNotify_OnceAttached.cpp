#include "Effect/API/AnimNotify/FXNotify_OnceAttached.h"
#include "Effect/API/FXSpawnerComponent.h"
#include "Effect/Util/FXUtil.h"

void UFXNotify_OnceAttached::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Anim)
{
	if (AActor* Owner = GetResolveOwner(Mesh))
	{
		if (UFXSpawnerComponent* Spawner = GetResolveSpawner(Owner))
		{
			Spawner->SpawnOnceAttached(Tag, AttachInfo, ScaleOverride, LifeTimeOverride);
		}
	}
}