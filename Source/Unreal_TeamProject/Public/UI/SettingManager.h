#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SettingManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UNREAL_TEAMPROJECT_API USettingManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//SettingManager를 전역적으로 가져오는 함수
	static USettingManager* GetSettingManager(UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "UMG")
	void SetisToonGraphic(bool _bIsToon);

	UFUNCTION(BlueprintCallable, Category = "UMG")
	bool GetisToonGraphic() const { return m_bIsToonGraphic; }

	UFUNCTION(BlueprintCallable, Category = "UMG")
	void SetisMotionBlur(bool _bIsMotionBlur);

	UFUNCTION(BlueprintCallable, Category = "UMG")
	bool GetisMotionBlur() const { return m_bIsMotionBlur; }

private:

	bool m_bIsToonGraphic = false;
	bool m_bIsMotionBlur = true;
};
