#include "Item/ItemManager.h"
#include "Item/Item_Actor.h"

bool UItemManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (const UWorld* world = Cast<UWorld>(Outer))
	{
		if (!world->IsGameWorld())
		{
			return false;
		}

		return true;
	}
	return false;
}

void UItemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//UE_LOG(LogTemp, Warning, TEXT("ItemManager Initialized"));

	UWorld* world = GetWorld();
	if (!world || !world->IsGameWorld())
	{
		return;
	}

	if (!Item_ActorClass)
	{
		Item_ActorClass = LoadClass<AItem_Actor>(nullptr, TEXT("/Game/Item/Blueprint/BP_Item_Actor.BP_Item_Actor_C"));
	}
	
	if (Item_ActorClass && GetWorld())
	{
		int a = 0;
		for (int i = 0; i < Item_ActorPoolSize; i++)
		{
			AItem_Actor* Item = GetWorld()->SpawnActor<AItem_Actor>(Item_ActorClass);
			if (Item)
			{
				if (!Item->OnFinished.IsAlreadyBound(this, &UItemManager::OnExpFinished))
				{
					Item->OnFinished.AddDynamic(this, &UItemManager::OnExpFinished);
				}
				Item->SetActorHiddenInGame(true);
				Item->SetActorTickEnabled(false);
				InActive_ActorPool.Add(Item);
	
				//UE_LOG(LogTemp, Warning, TEXT("Actor Create! %d (%s)"),
				//	++a, *GetNameSafe(A));  // ← %d 자리에 숫자, %s 자리에 문자열
			}
		}
	}
}

void UItemManager::Deinitialize()
{
	Super::Deinitialize();

	

	for (TWeakObjectPtr<AItem_Actor>& W : InActive_ActorPool) 
	{
		if (AItem_Actor* Actor = W.Get())                    
		{
			Actor->Destroy();
		}
	}
	InActive_ActorPool.Empty();

	for (TWeakObjectPtr<AItem_Actor>& W : Active_ActorPool) 
	{
		if (AItem_Actor* Actor = W.Get())                    
		{
			Actor->Destroy();
		}
	}
	Active_ActorPool.Empty();

	//UE_LOG(LogTemp, Warning, TEXT("ItemManager Deinitialized (pools cleared)"));
}

void UItemManager::Set_Item_Exp(int32 ExpAmount, FVector Location)
{
	AItem_Actor* Item = GetPooledItemActor();
	if (!Item)
	{
		return;
	}
	m_iRemain_Exp = ExpAmount;
	float fExpAmount = static_cast<float>(ExpAmount);

	while (m_iRemain_Exp > 0)
	{
		if (m_iRemain_Exp > 4)
		{
			fExpAmount = 5.0f;
			m_iRemain_Exp -= 5;
		}
		else
		{
			fExpAmount = static_cast<float>(m_iRemain_Exp);
			m_iRemain_Exp = 0;
		}

		Item->SetItem_Exp(fExpAmount, Location);
		Item = GetPooledItemActor();
		if (!Item)
		{
			return;
		}
	}


}

UItemManager* UItemManager::GetItemManager(UWorld* World)
{
	if (nullptr == World)
	{
		return nullptr;
	}

	return World->GetSubsystem<UItemManager>();
}

void UItemManager::OnExpFinished(AItem_Actor* Item)
{
	Release(Item);
}

AItem_Actor* UItemManager::GetPooledItemActor()
{
	//UE_LOG(LogTemp, Warning, TEXT("Item InActive_Pool Count %d"), InActive_ActorPool.Num());  // ← %d 자리에 숫자, %s 자리에 문자열
	//UE_LOG(LogTemp, Warning, TEXT("Item Active_Pool Count %d"), Active_ActorPool.Num());  // ← %d 자리에 숫자, %s 자리에 문자열

	while (InActive_ActorPool.Num() > 0 && !InActive_ActorPool.Last().IsValid())
	{
		InActive_ActorPool.Pop();
	}

	//풀에 남은게 있으면 그걸 사용
	if (InActive_ActorPool.Num() > 0)
	{
		AItem_Actor* item = InActive_ActorPool.Pop().Get();
		Active_ActorPool.Add(item);
		UE_LOG(LogTemp, Warning, TEXT("Item_ActorClass Using Old One."));
		return item;
	}

	//풀에 남은게 없으면 새로 생성
	if (Item_ActorClass && GetWorld())
	{
		AItem_Actor* item = GetWorld()->SpawnActor<AItem_Actor>(Item_ActorClass);
		if (!item->OnFinished.IsAlreadyBound(this, &UItemManager::OnExpFinished))
		{
			item->OnFinished.AddDynamic(this, &UItemManager::OnExpFinished);
		}
		Active_ActorPool.Add(item);

		//UE_LOG(LogTemp, Warning, TEXT("Item_ActorClass Create New One."));
		return item;
	}
	return nullptr;
}

void UItemManager::Release(AItem_Actor* Item)
{
	if (!Item)
	{
		return;
	}

	Active_ActorPool.RemoveSwap(Item);
	Item->SetActorHiddenInGame(true);
	Item->SetActorTickEnabled(false);
	InActive_ActorPool.Add(Item);
	//UE_LOG(LogTemp, Warning, TEXT("ItemManager Release"));
}