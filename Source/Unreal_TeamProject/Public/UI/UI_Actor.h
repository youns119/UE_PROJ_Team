#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UIEnum.h"
#include "UI_Actor.generated.h"

class UWidgetComponent;  
class UDamageFont;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageUiFinished, AUI_Actor*, Actor);

UCLASS()
class UNREAL_TEAMPROJECT_API AUI_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUI_Actor();

	void SetDamage(float DamageAmount, FVector Location, EUIDamageType eDamageType);
	
	UPROPERTY(BlueprintAssignable)
	FOnDamageUiFinished OnFinished;

	UFUNCTION(BlueprintImplementableEvent, Category = "UMG")
	void MoveDamageFont();




public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* m_WidgetComp;

	FVector m_Position;
	TWeakObjectPtr<APlayerController> m_PlayerController;

	float m_LifeTime = 10.0f;
	//float m_ElapsedTime = 5.0f;


	UPROPERTY() class UDamageFont* m_DamageWidget;
	bool bIsActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
	FVector m_SpawnCurvePosition;

};
