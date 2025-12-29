#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USceneComponent;

class IFXService
{
public :
	virtual ~IFXService() = default;

// ==== Once ====
public :
	virtual UNiagaraComponent* SpawnOnceLocation(UNiagaraSystem* FX, const FTransform& Transform, float Scale) = 0;
	virtual UNiagaraComponent* SpawnOnceAttached
	(
		UNiagaraSystem* FX,
		const FGameplayTag& Tag,
		USceneComponent* Target,
		FName SocketName, 
		const FTransform& Offset,
		float Scale, 
		UActorComponent* Owner = nullptr
	) = 0;

// ==== Attached ====
public :
	virtual UNiagaraComponent* SpawnLoopAttached
	(
		UNiagaraSystem* FX,
		const FGameplayTag& Tag,
		USceneComponent* Target,
		FName SocketName,
		const FTransform& Offset,
		float Scale,
		bool& bReused,
		bool& bReplaced,
		UActorComponent* Owner = nullptr
	) = 0;

public :
	virtual void ReleaseAfter(UNiagaraComponent* FX, float Time) = 0;
};