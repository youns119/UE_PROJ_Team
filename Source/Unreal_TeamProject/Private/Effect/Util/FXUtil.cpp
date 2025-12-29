#include "Effect/Util/FXUtil.h"
#include "Effect/Data/FXData.h"
#include "Effect/Core/DataType/FXType.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

namespace FXUtil
{
	void ApplyRelativeOffset(USceneComponent* SceneComponent, const FTransform& Transform, float Scale)
	{
		if (!SceneComponent)
		{
			return;
		}

		SceneComponent->SetRelativeTransform(Transform);
		SceneComponent->SetRelativeScale3D(Transform.GetScale3D() * FVector(Scale));
	}

	FTransform GetFinalTransform(FTransform& Base, FTransform& Offset)
	{
		Base.SetScale3D(FVector::OneVector);
		Offset.SetScale3D(FVector::OneVector);

		return Base * Offset;
	}

	USceneComponent* GetTargetFromReference(AActor* Owner, const FComponentReference& ComponentRef)
	{
		if (AActor* Other = ComponentRef.OtherActor.Get())
		{
			if (UActorComponent* ActorComponent = ComponentRef.GetComponent(Other))
			{
				if (auto* SceneComponent = Cast<USceneComponent>(ActorComponent))
				{
					return SceneComponent;
				}
			}
		}

		if (!Owner)
		{
			return nullptr;
		}

		if (UActorComponent* ActorComponent = ComponentRef.GetComponent(Owner))
		{
			if (auto* SceneComponent = Cast<USceneComponent>(ActorComponent))
			{
				return SceneComponent;
			}
		}

		return nullptr;
	}

	USceneComponent* GetResolveAttachTarget
	(
		AActor* Owner,
		const FComponentReference& TargetRef,
		const FComponentReference& FallbackRef,
		bool bFallback
	)
	{
		if (USceneComponent* Target = GetTargetFromReference(Owner, TargetRef))
		{
			return Target;
		}

		if (USceneComponent* Target = GetTargetFromReference(Owner, FallbackRef))
		{
			return Target;
		}

		if (!Owner)
		{
			return nullptr;
		}

		if (bFallback)
		{
			if (auto* Character = Cast<ACharacter>(Owner))
			{
				if (auto* Mesh = Character->GetMesh())
				{
					return Mesh;
				}
			}
		}

		return Owner->GetRootComponent();
	}

	USceneComponent* GetResolveAttachTarget
	(
		AActor* Owner, 
		USceneComponent* Target, 
		const FComponentReference& FallbackRef, 
		bool bFallback
	)
	{
		if (Target)
		{
			return Target;
		}

		if (USceneComponent* SceneComponent = GetTargetFromReference(Owner, FallbackRef))
		{
			return SceneComponent;
		}

		if (!Owner)
		{
			return nullptr;
		}

		if (bFallback)
		{
			if (auto* Character = Cast<ACharacter>(Owner))
			{
				if (auto* Mesh = Character->GetMesh())
				{
					return Mesh;
				}
			}
		}

		return Owner->GetRootComponent();
	}

	USkeletalMeshComponent* GetResolveMesh(AActor* Owner)
	{
		return Owner ? Owner->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
	}

	FName GetResolveSocketName(FName SocketName, FName Fallback)
	{
		if (!SocketName.IsNone())
			return SocketName;

		return Fallback.IsNone() ? NAME_None : Fallback;
	}

	float GetResolveScale(const UFXData* Data, float ScaleOverride)
	{
		return (ScaleOverride >= 0.f && Data) ? ScaleOverride
			: (Data ? Data->GetBaseScale() : 0.f);
	}

	float GetResolveLifeTime(const UFXData* Data, float LifeTimeOverride)
	{
		return (LifeTimeOverride >= 0.f && Data) ? LifeTimeOverride
			: (Data ? Data->GetLifeTime() : 0.f);
	}

	FTransform GetResolveTransform(const FFXWorldPlacement& WorldPlacement, AActor* Owner)
	{
		switch (WorldPlacement.PlaceType)
		{
		case EFXWorldPlacement::Owner:
		{
			return Owner ? Owner->GetActorTransform() : FTransform::Identity;
		}
		case EFXWorldPlacement::Component:
		case EFXWorldPlacement::Socket:
		{
			USceneComponent* Component = FXUtil::GetResolveAttachTarget(Owner, nullptr, WorldPlacement.ComponentRef, true);
			if (!Component)
			{
				return FTransform::Identity;
			}

			if (WorldPlacement.PlaceType == EFXWorldPlacement::Socket && WorldPlacement.SocketName != NAME_None)
			{
				return Component->GetSocketTransform(WorldPlacement.SocketName, ERelativeTransformSpace::RTS_World);
			}

			return Component->GetComponentTransform();
		}
		case EFXWorldPlacement::Custom:
		{
			return WorldPlacement.CustomTransform;
		}
		default:
		{
			return FTransform::Identity;
		}
		}
	}

	USceneComponent* FindTargetComponent(AActor* Owner, TSubclassOf<USceneComponent> Class, FName Name)
	{
		if (!Owner)
		{
			return nullptr;
		}

		TArray<UActorComponent*> RawComponent{};
		Owner->GetComponents(Class ? *Class : USceneComponent::StaticClass(), RawComponent);

		for (UActorComponent* ActorComponent : RawComponent)
		{
			USceneComponent* SceneComponent = Cast<USceneComponent>(ActorComponent);
			if (!SceneComponent)
			{
				continue;
			}

			const bool bName = (Name == NAME_None) || (SceneComponent->GetFName() == Name);
			if (bName)
			{
				return SceneComponent;
			}
		}

		if (ACharacter* Character = Cast<ACharacter>(Owner))
		{
			if (USceneComponent* SceneComponent = Character->GetMesh())
			{
				return SceneComponent;
			}
		}

		return Owner->GetRootComponent();
	}
}