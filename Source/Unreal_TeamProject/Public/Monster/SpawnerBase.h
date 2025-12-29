#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnerBase.generated.h"

UCLASS()
class UNREAL_TEAMPROJECT_API ASpawnerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnerBase();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintImplementableEvent, Category = "Wave")
	void BeginSpawning();
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
