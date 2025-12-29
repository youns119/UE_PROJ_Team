#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WaveManager.generated.h"

USTRUCT()
struct FSpawnerArray
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<TObjectPtr<class ASpawnerBase>> SpawnerArray;
};

UCLASS(Blueprintable)
class UNREAL_TEAMPROJECT_API UWaveManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//SubSystem 생성 여부 결정
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//SubSystem 생성 시 호출
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//SubSystem 제거 시 호출
	virtual void Deinitialize() override;

	static UWaveManager* GetWaveManager(UWorld* World);
	
	UFUNCTION(BlueprintCallable, Category = "Wave")
	int32 Get_Wave();

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void Set_Wave(int32 _iWave);

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void AllMonsterClear();

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void SpawningComplete();

private:
	void Setting_SpawnerList();
	void Start_Wave();
	int32 Get_TagIndex(const TArray<FName>& _tagName);

	UPROPERTY(Transient)
	TArray<FSpawnerArray> m_SpawnerList;

	int32 m_iWave = 0;
	int32 m_iSpawnerCount = 0;
	bool m_bSpawningComplete = false;
	bool m_bAllMonsterClear = false;
};
