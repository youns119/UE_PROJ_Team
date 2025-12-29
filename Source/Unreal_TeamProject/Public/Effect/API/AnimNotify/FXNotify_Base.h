#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Effect/Core/DataType/FXType.h"
#include "GameplayTagContainer.h"
#include "FXNotify_Base.generated.h"

class USceneComponent;
class UFXSpawnerComponent;

UCLASS(meta = (DisplayName = "FX : FX Notify Base"))
class UNREAL_TEAMPROJECT_API UFXNotify_Base 
	: public UAnimNotify
{
	GENERATED_BODY()
	
protected :
    AActor* GetResolveOwner(USkeletalMeshComponent* Mesh);
    UFXSpawnerComponent* GetResolveSpawner(AActor* Owner);

protected :
    UPROPERTY(EditAnywhere, Category = "FX")
    FGameplayTag Tag{};

    UPROPERTY(EditAnywhere, Category = "FX")
    float ScaleOverride{ -1.f };

    UPROPERTY(EditAnywhere, Category = "FX")
    float LifeTimeOverride{ -1.f };

    UPROPERTY(EditAnywhere, Category = "FX")
    TObjectPtr<UFXSpawnerComponent> SpawnerOverride{};
};