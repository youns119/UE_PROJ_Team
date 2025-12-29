#pragma once

#include "CoreMinimal.h"
#include "Effect/Core/DataType/FXType.h"

class USceneComponent;
class USkeletalMeshComponent;
class AActor;
class UFXData;
class FName;
struct FComponentReference;

namespace FXUtil
{
	// Calculate and apply relative transform offset to the component;
	void ApplyRelativeOffset(USceneComponent* SceneComponent, const FTransform& Transform, float Scale);

	FTransform GetFinalTransform(FTransform& Base, FTransform& Offset);

	// Get Attach Target
	USceneComponent* GetTargetFromReference(AActor* Owner, const FComponentReference& ComponentRef);

	// Get Resolved Target
	USceneComponent* GetResolveAttachTarget
	(
		AActor* Owner,
		const FComponentReference& TargetRef,
		const FComponentReference& FallbackRef,
		bool bFallback = true
	);

	// Get Resolved Target
	USceneComponent* GetResolveAttachTarget
	(
		AActor* Owner,
		USceneComponent* Target,
		const FComponentReference& FallbackRef,
		bool bFallback = true
	);

	// Get Resolved Mesh
	USkeletalMeshComponent* GetResolveMesh(AActor* Owner);

	// Get Resolved Socket Name
	FName GetResolveSocketName(FName SocketName, FName Fallback = NAME_None);

	// Get Resolved Scale
	float GetResolveScale(const UFXData* Data, float ScaleOverride);

	// Get Resolved LifeTime
	float GetResolveLifeTime(const UFXData* Data, float LifeTimeOverride);

	// Get World Base Transform
	FTransform GetResolveTransform(const FFXWorldPlacement& WorldPlacement, AActor* Owner);

	// Find Target Component
	USceneComponent* FindTargetComponent(AActor* Owner, TSubclassOf<USceneComponent> Class, FName Name);
}