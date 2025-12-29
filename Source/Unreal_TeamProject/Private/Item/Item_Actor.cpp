#include "Item/Item_Actor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem_Actor::AItem_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

void AItem_Actor::SetItem_Exp(float fExpAmount, FVector Location)
{
	m_Position = Location;

	//UE_LOG(LogTemp, Log, TEXT("SetItem called! Location: %s"), *Location.ToString());

	//FVector v = { 100.f, 100.f, 100.f };
	//SetActorLocation(m_Position + v);
	
	SetActorLocation(m_Position);

	Setting_SpawnCurveVector();

	bIsActive = true;
	m_LifeTime = 10.0f;
	m_fExpAmount = fExpAmount;
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
}

void AItem_Actor::ReleaseItem()
{
	m_LifeTime = 0.0f;
}

// Called every frame
void AItem_Actor::Tick(float DeltaTime)
{
	if (!bIsActive)
	{
		return;
	}
	Super::Tick(DeltaTime);

	m_LifeTime -= DeltaTime;

	if (m_LifeTime < 0.0f)
	{
		bIsActive = false;
		ExpToPlayer();
		OnFinished.Broadcast(this);
	}
}

