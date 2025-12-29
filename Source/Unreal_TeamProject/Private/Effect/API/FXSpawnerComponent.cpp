#include "Effect/API/FXSpawnerComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Effect/System/FXRegistrySubsystem.h"
#include "Effect/System/FXWorldSubsystem.h"
#include "Effect/Data/FXData.h"
#include "Effect/Util/FXUtil.h"

UFXSpawnerComponent::UFXSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFXSpawnerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		if (UFXWorldSubsystem* FXWorldSubsystem = World->GetSubsystem<UFXWorldSubsystem>())
		{
			FXWorldSubsystem->StopAllByOwner(this, EFXStopMode::Immediate);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UFXSpawnerComponent::SpawnOnceLocation
(
	FGameplayTag Tag,
	const FFXWorldPlacement& PlaceType,
	float ScaleOverride,
	float LifeTimeOverride
)
{
	if (const UWorld* World = GetWorld())
	{
		if (UFXWorldSubsystem* FXWorldSubsystem = World->GetSubsystem<UFXWorldSubsystem>())
		{
			FTransform Base = FXUtil::GetResolveTransform(PlaceType, GetOwner());
			FTransform Offset = PlaceType.RelativeOffset;

			FFXCueParams_World Param{};
			Param.WorldTransform = FXUtil::GetFinalTransform(Base, Offset);
			Param.Scale = ScaleOverride;
			Param.LifeTimeOverride = LifeTimeOverride;

			FXWorldSubsystem->FireCueOnce_AtLocation(Tag, Param);
		}
	}
}

void UFXSpawnerComponent::SpawnOnceAttached
(
	FGameplayTag Tag,
	FFXAttachInfo AttachInfo,
	float ScaleOverride,
	float LifeTimeOverride
)
{
	if (const UWorld* World = GetWorld())
	{
		if (UFXWorldSubsystem* FXWorldSubsystem = World->GetSubsystem<UFXWorldSubsystem>())
		{
			FFXCueParams_Attached Param{};
			Param.AttachInfo.ComponentRef = DefaultTarget;
			Param.AttachInfo.SocketName = FXUtil::GetResolveSocketName(AttachInfo.SocketName, DefaultSocketName);
			Param.AttachInfo.RelativeOffset = AttachInfo.RelativeOffset;
			Param.Scale = ScaleOverride;
			Param.LifeTimeOverride = LifeTimeOverride;
			Param.AttachTarget = FXUtil::GetResolveAttachTarget(GetOwner(), AttachInfo.ComponentRef, DefaultTarget, true);
			Param.Owner = this;

			FXWorldSubsystem->FireCueOnce_Attached(Tag, Param);
		}
	}
}

void UFXSpawnerComponent::SpawnLoopAttached
(
	FGameplayTag Tag,
	FFXAttachInfo AttachInfo,
	float ScaleOverride,
	float LifeTimeOverride,
	bool bRestart
)
{
	if (const UWorld* World = GetWorld())
	{
		if (UFXWorldSubsystem* FXWorldSubsystem = World->GetSubsystem<UFXWorldSubsystem>())
		{
			FFXCueParams_Attached Param{};
			Param.AttachInfo.ComponentRef = DefaultTarget;
			Param.AttachInfo.SocketName = FXUtil::GetResolveSocketName(AttachInfo.SocketName, DefaultSocketName);
			Param.AttachInfo.RelativeOffset = AttachInfo.RelativeOffset;
			Param.Scale = ScaleOverride;
			Param.LifeTimeOverride = LifeTimeOverride;
			Param.bRestart = bRestart;
			Param.AttachTarget = FXUtil::GetResolveAttachTarget(GetOwner(), AttachInfo.ComponentRef, DefaultTarget, true);
			Param.Owner = this;

			FXWorldSubsystem->FireCueLoop_Attached(Tag, Param);
		}
	}
}

void UFXSpawnerComponent::StopAllOnSocket
(
	const FComponentReference& Target,
	FName SocketName,
	EFXStopMode Mode
)
{
	if (const UWorld* World = GetWorld())
	{
		if (UFXWorldSubsystem* FXWorldSubsystem = World->GetSubsystem<UFXWorldSubsystem>())
		{
			if (USceneComponent* FinalTarget = FXUtil::GetResolveAttachTarget(GetOwner(), Target, DefaultTarget, true))
			{
				const FName Socket = FXUtil::GetResolveSocketName(SocketName, DefaultSocketName);
				FXWorldSubsystem->StopAllOnSocket(FinalTarget, Socket, Mode);
			}
		}
	}
}

void UFXSpawnerComponent::StopByTagOnSocket
(
	FGameplayTag Tag,
	const FComponentReference& Target,
	FName SocketName,
	EFXStopMode Mode
)
{
	if (const UWorld* World = GetWorld())
	{
		if (UFXWorldSubsystem* FXWorldSubsystem = World->GetSubsystem<UFXWorldSubsystem>())
		{
			if (USceneComponent* FinalTarget = FXUtil::GetResolveAttachTarget(GetOwner(), Target, DefaultTarget, true))
			{
				const FName Socket = FXUtil::GetResolveSocketName(SocketName, DefaultSocketName);
				FXWorldSubsystem->StopByTagOnSocket(Tag, FinalTarget, Socket, Mode);
			}
		}
	}
}

void UFXSpawnerComponent::StopAfterByTagOnSocket
(
	FGameplayTag Tag,
	const FComponentReference& Target,
	FName SocketName,
	float Time,
	EFXStopMode Mode
)
{
	if (const UWorld* World = GetWorld())
	{
		if (UFXWorldSubsystem* FXWorldSubsystem = World->GetSubsystem<UFXWorldSubsystem>())
		{
			if (USceneComponent* FinalTarget = FXUtil::GetResolveAttachTarget(GetOwner(), Target, DefaultTarget, true))
			{
				const FName Socket = FXUtil::GetResolveSocketName(SocketName, DefaultSocketName);
				FXWorldSubsystem->StopAfterByTagOnSocket(Tag, FinalTarget, Socket, Time, Mode);
			}
		}
	}
}

void UFXSpawnerComponent::SetOffsetByTagOnSocket
(
	FGameplayTag Tag,
	const FComponentReference& Target,
	FName SocketName,
	const FTransform& Offset,
	float ScaleOverride
)
{
	if (const UWorld* World = GetWorld())
	{
		if (UFXWorldSubsystem* FXWorldSubsystem = World->GetSubsystem<UFXWorldSubsystem>())
		{
			if (USceneComponent* FinalTarget = FXUtil::GetResolveAttachTarget(GetOwner(), Target, DefaultTarget, true))
			{
				const FName Socket = FXUtil::GetResolveSocketName(SocketName, DefaultSocketName);
				FXWorldSubsystem->SetOffsetByTagOnSocket(Tag, FinalTarget, Socket, Offset, ScaleOverride);
			}
		}
	}
}

bool UFXSpawnerComponent::HasLoopOnSocket(FGameplayTag Tag, const FComponentReference& Target, FName SocketName) const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UFXWorldSubsystem* FXWorldSubsystem = World->GetSubsystem<UFXWorldSubsystem>())
		{
			if (USceneComponent* FinalTarget = FXUtil::GetResolveAttachTarget(GetOwner(), Target, DefaultTarget, true))
			{
				const FName Socket = FXUtil::GetResolveSocketName(SocketName, DefaultSocketName);
				return FXWorldSubsystem->HasLoopOnSocket(Tag, FinalTarget, Socket);
			}
		}
	}

	return false;
}

bool UFXSpawnerComponent::HasAnyOnSocket(FGameplayTag Tag, const FComponentReference& Target, FName SocketName) const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UFXWorldSubsystem* FXWorldSubsystem = World->GetSubsystem<UFXWorldSubsystem>())
		{
			if (USceneComponent* FinalTarget = FXUtil::GetResolveAttachTarget(GetOwner(), Target, DefaultTarget, true))
			{
				const FName Socket = FXUtil::GetResolveSocketName(SocketName, DefaultSocketName);
				return FXWorldSubsystem->HasAnyOnSocket(Tag, FinalTarget, Socket);
			}
		}
	}
	return false;
}