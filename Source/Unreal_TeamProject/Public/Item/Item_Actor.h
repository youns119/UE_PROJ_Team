#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item_Actor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExpFinished, AItem_Actor*, Actor);

UCLASS()
class UNREAL_TEAMPROJECT_API AItem_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem_Actor();

	void SetItem_Exp(float fExpAmount, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void ReleaseItem();

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void ExpToPlayer();

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void Setting_SpawnCurveVector();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FOnExpFinished OnFinished;

	FVector m_Position;
	float m_LifeTime = 10.0f;

	bool bIsActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float m_fExpAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float m_fMoveSpeed = 1300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float m_fSpawnSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FVector m_SpawnCurvePosition;
};
