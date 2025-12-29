#include "UI/SettingManager.h"

void USettingManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//UE_LOG(LogTemp, Warning, TEXT("SettingManager Initialized"));
}

void USettingManager::Deinitialize()
{
	Super::Deinitialize();
	//UE_LOG(LogTemp, Warning, TEXT("SettingManager Deinitialized"));
}

USettingManager* USettingManager::GetSettingManager(UWorld* World)
{
	if (nullptr == World)
	{
		return nullptr;
	}
	else
	{
		return World->GetGameInstance()->GetSubsystem<USettingManager>();
	}

}

void USettingManager::SetisToonGraphic(bool _bIsToon)
{
	m_bIsToonGraphic = _bIsToon;
}

void USettingManager::SetisMotionBlur(bool _bIsMotionBlur)
{
	m_bIsMotionBlur = _bIsMotionBlur;
}