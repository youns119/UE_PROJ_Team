#include "UI/UIManager.h"
#include "UI/DamageFont.h"
#include "UI/UI_Actor.h"
#include "UI/UIEnum.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//UE_LOG(LogTemp, Warning, TEXT("UIManager Initialized"));

	if (!UI_ActorClass)
	{
		UI_ActorClass = LoadClass<AUI_Actor>(nullptr, TEXT("/Game/UI/Blueprint/BP_UI_Actor.BP_UI_Actor_C"));
	}

	if (!UI_GPSClass)
	{
		UI_GPSClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/UMG_Widget/Minimap/UMG_MinimapMonster.UMG_MinimapMonster_C"));
	}

	if (!UI_GPSClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("GPS_Widget is not set or World is null."));
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("GPS_Widget is set."));
	}

	if (UI_ActorClass && GetWorld())
	{
		int a = 0;
		for (int i = 0; i < UI_ActorPoolSize; i++)
		{
			AUI_Actor* Actor = GetWorld()->SpawnActor<AUI_Actor>(UI_ActorClass);
			if (Actor)
			{
				if (!Actor->OnFinished.IsAlreadyBound(this, &UUIManager::OnDamageUiFinished))
				{
					Actor->OnFinished.AddDynamic(this, &UUIManager::OnDamageUiFinished);
				}
				Actor->SetActorHiddenInGame(true);
				Actor->SetActorTickEnabled(false);
				InActive_ActorPool.Add(Actor);

				//UE_LOG(LogTemp, Warning, TEXT("Actor Create! %d (%s)"),
				//	++a, *GetNameSafe(Actor));  // ← %d 자리에 숫자, %s 자리에 문자열
			}
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("UI_ActorClass is not set or World is null."));
	}

	
}

void UUIManager::Deinitialize()
{
	Super::Deinitialize();


	for (TObjectPtr<AUI_Actor>& iter : InActive_ActorPool)
	{
		if (AUI_Actor* Actor = iter.Get())
		{
			Actor->Destroy();
		}
	}
	InActive_ActorPool.Empty();

	for (TObjectPtr<AUI_Actor>& iter : Active_ActorPool)
	{
		if (AUI_Actor* Actor = iter.Get())
		{
			Actor->Destroy();
		}
	}
	Active_ActorPool.Empty();


	for (TObjectPtr<UUserWidget>& iter : InActive_GPSPool)
	{
		if (UUserWidget* Widget = iter.Get())
		{
			Widget->RemoveFromParent();
		}
	}
	InActive_GPSPool.Empty();

	for (TObjectPtr<UUserWidget>& iter : Active_GPSPool)
	{
		if (UUserWidget* Widget = iter.Get())
		{
			Widget->RemoveFromParent();
		}
	}
	Active_GPSPool.Empty();

	//UE_LOG(LogTemp, Warning, TEXT("UIManager Deinitialized"));
}

UUIManager* UUIManager::GetUIManager(UWorld* World)
{
	if (nullptr == World)
	{
		return nullptr;
	}

	if (ULocalPlayer* localPlayer = World->GetFirstLocalPlayerFromController())
	{
		return localPlayer->GetSubsystem<UUIManager>();
	}

	return nullptr;
}

void UUIManager::SetDamage_Monster(float DamageAmount, FVector Location, EUIDamageType eDamageType)
{
	//UE_LOG(LogTemp, Log, TEXT("SetDamage_Monster called!"));

	//과거 테스트용(UI만 잠깐 띄우는 용도였음)
	
	//if (!DamageWidget.IsValid())
	//{
	//	if (UClass* widgetClass = LoadClass<UDamageFont>(nullptr, TEXT("/Game/UI/UMG_Widget/UMG_DamageFont.UMG_DamageFont_C")))
	//	{
	//		UDamageFont* widget = CreateWidget<UDamageFont>(GetWorld(), widgetClass);
	//
	//		if (widget)
	//		{
	//			widget->AddToViewport();
	//
	//			DamageWidget = widget;
	//
	//			DamageWidget->ShowDamage(DamageAmount, Location);
	//		}
	//	}
	//}
	//
	//if (DamageWidget.IsValid())
	//{
	//	DamageWidget->ShowDamage(DamageAmount, Location);
	//}

	AUI_Actor* Actor = GetPooledUIActor();
	if (!Actor)
	{
		//UE_LOG(LogTemp, Warning, TEXT("No Actor!"));
		return;
	}



	Actor->SetDamage(DamageAmount, Location, eDamageType);

}


AUI_Actor* UUIManager::GetPooledUIActor()
{
	//UE_LOG(LogTemp, Warning, TEXT("InActive_Pool Count %d"), InActive_ActorPool.Num());  // ← %d 자리에 숫자, %s 자리에 문자열
	//UE_LOG(LogTemp, Warning, TEXT("Active_Pool Count %d"), Active_ActorPool.Num());  // ← %d 자리에 숫자, %s 자리에 문자열

	while (InActive_ActorPool.Num() > 0 && !InActive_ActorPool.Last().Get())
	{
		InActive_ActorPool.Pop();
	}

	//풀에 남은게 있으면 그걸 사용
	if (InActive_ActorPool.Num() > 0)
	{
		AUI_Actor* Actor = InActive_ActorPool.Pop().Get();
		Active_ActorPool.Add(Actor);
		return Actor;
	}

	//풀에 남은게 없으면 새로 생성
	if (UI_ActorClass && GetWorld())
	{
		AUI_Actor* Actor = GetWorld()->SpawnActor<AUI_Actor>(UI_ActorClass);
		if (!Actor->OnFinished.IsAlreadyBound(this, &UUIManager::OnDamageUiFinished))
		{
			Actor->OnFinished.AddDynamic(this, &UUIManager::OnDamageUiFinished);
		}
		Active_ActorPool.Add(Actor);
		return Actor;
	}
	return nullptr;
}

UUserWidget* UUIManager::GetPooledUIGPS()
{
	//UE_LOG(LogTemp, Warning, TEXT("InActive_GPSPool Count %d"), InActive_GPSPool.Num());  // ← %d 자리에 숫자, %s 자리에 문자열
	//UE_LOG(LogTemp, Warning, TEXT("Active_GPSPool Count %d"), Active_GPSPool.Num());  // ← %d 자리에 숫자, %s 자리에 문자열

	while (InActive_GPSPool.Num() > 0 && !InActive_GPSPool.Last().Get())
	{
		InActive_GPSPool.Pop();
	}

	//풀에 남은게 있으면 그걸 사용
	if (InActive_GPSPool.Num() > 0)
	{
		UUserWidget* Widget = InActive_GPSPool.Pop().Get();
		Active_GPSPool.Add(Widget);
		return Widget;
	}

	//풀에 남은게 없으면 새로 생성
	if (UI_GPSClass && GetWorld())
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			UUserWidget* Widget = CreateWidget<UUserWidget>(PC, UI_GPSClass);
			Active_GPSPool.Add(Widget);
			return Widget;
		}
	}
	return nullptr;
}

void UUIManager::OnDamageUiFinished(AUI_Actor* Actor)
{
	Release(Actor);
}

void UUIManager::Release(AUI_Actor* Actor)
{
	if (!Actor)
	{
		return;
	}

	Active_ActorPool.RemoveSwap(Actor);
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorTickEnabled(false);
	InActive_ActorPool.Add(Actor);
	//UE_LOG(LogTemp, Warning, TEXT("UIManager Release"));
}

void UUIManager::ReleaseGPS(UUserWidget* GPS)
{
	if (!GPS)
	{
		return;
	}

	Active_GPSPool.RemoveSwap(GPS);
	//GPS->ReleaseToPool();
	
	InActive_GPSPool.Add(GPS);
	//UE_LOG(LogTemp, Warning, TEXT("UIManager Release"));
}

void UUIManager::ReadyGPSPool()
{
	if (UI_GPSClass && GetWorld())
	{
		int a = 0;
		for (int i = 0; i < UI_ActorPoolSize; i++)
		{
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				UUserWidget* GPS = CreateWidget<UUserWidget>(PC, UI_GPSClass);
				if (GPS)
				{
					InActive_GPSPool.Add(GPS);

					//UE_LOG(LogTemp, Warning, TEXT("GPS_Widget Create! %d (%s)"),
					//	++a, *GetNameSafe(GPS));  // ← %d 자리에 숫자, %s 자리에 문자열
				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("Failed to create GPS_Widget!"));
				}
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Failed to create GPS_Widget! because no PC"));
			}

		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("GPS_Widget is not set or World is null."));
	}
}