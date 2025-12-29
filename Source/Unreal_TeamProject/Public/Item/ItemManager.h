#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ItemManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UNREAL_TEAMPROJECT_API UItemManager : public UWorldSubsystem
{
	GENERATED_BODY()
	

public:
	//SubSystem 생성 여부 결정
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//SubSystem 생성 시 호출
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//SubSystem 제거 시 호출
	virtual void Deinitialize() override;

	static UItemManager* GetItemManager(UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void Set_Item_Exp(int32 ExpAmount, FVector Location);

	UPROPERTY(EditAnywhere, Category = "Item")
	TSubclassOf<class AItem_Actor> Item_ActorClass;

	UPROPERTY(EditAnywhere, Category = "Item")
	int32 Item_ActorPoolSize = 16;

	UFUNCTION()
	void OnExpFinished(AItem_Actor* Item);

private:

	TArray<TWeakObjectPtr<class AItem_Actor>> Active_ActorPool;
	TArray<TWeakObjectPtr<class AItem_Actor>> InActive_ActorPool;

	AItem_Actor* GetPooledItemActor();
	int32 m_iRemain_Exp = 0;


	void Release(AItem_Actor* Item);
};
