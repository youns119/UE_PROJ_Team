#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UIManager.generated.h"
/**
 * 
 */
UCLASS(Blueprintable)
class UNREAL_TEAMPROJECT_API UUIManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	//SubSystem 생성 시 호출
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//SubSystem 제거 시 호출
	virtual void Deinitialize() override;

	//UIManager를 전역적으로 가져오는 함수
	static UUIManager* GetUIManager(UWorld* UWorld);

	UFUNCTION(BlueprintCallable, Category = "UMG")
	void SetDamage_Monster(float DamageAmount, FVector Location, EUIDamageType eDamageType);

	UPROPERTY(EditAnywhere, Category = "UMG")
	TSubclassOf<class AUI_Actor> UI_ActorClass;

	UPROPERTY(EditAnywhere, Category = "UMG")
	TSubclassOf<class UUserWidget> UI_GPSClass;

	UPROPERTY(EditAnywhere, Category = "UMG")
	int32 UI_ActorPoolSize = 10;

	UFUNCTION()
	void OnDamageUiFinished(AUI_Actor* Actor);

	UFUNCTION(BlueprintCallable, Category = "UMG")
	void ReadyGPSPool();

	UFUNCTION(BlueprintCallable, Category = "UMG")
	UUserWidget* GetPooledUIGPS();

	UFUNCTION(BlueprintCallable, Category = "UMG")
	void ReleaseGPS(UUserWidget* GPS);

private:

	UPROPERTY() TArray<TObjectPtr<class AUI_Actor>> Active_ActorPool;
	UPROPERTY() TArray<TObjectPtr<class AUI_Actor>> InActive_ActorPool;

	UPROPERTY() TArray<TObjectPtr<class UUserWidget>> Active_GPSPool;
	UPROPERTY() TArray<TObjectPtr<class UUserWidget>> InActive_GPSPool;

	AUI_Actor* GetPooledUIActor();
	

	void Release(AUI_Actor* Actor);
	
};