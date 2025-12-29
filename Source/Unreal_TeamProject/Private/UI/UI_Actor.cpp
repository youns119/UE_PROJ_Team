#include "UI/UI_Actor.h"
#include "Components/WidgetComponent.h"
#include "UI/DamageFont.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AUI_Actor::AUI_Actor()
{
	PrimaryActorTick.bCanEverTick = true;

	m_WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	SetRootComponent(m_WidgetComp);

	m_WidgetComp->SetWidgetSpace(EWidgetSpace::World);
	m_WidgetComp->SetDrawAtDesiredSize(true);
	m_WidgetComp->SetTwoSided(true);
	m_WidgetComp->SetPivot(FVector2D(0.5f, 0.5f));

	// 초기엔 풀 상태라 가려두기
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

void AUI_Actor::SetDamage(float DamageAmount, FVector Location, EUIDamageType eDamageType)
{
	m_Position = Location;

	//UE_LOG(LogTemp, Log, TEXT("SetDamage called! Location: %s"), *Location.ToString());

	SetActorLocation(m_Position);

	MoveDamageFont();

	if (!m_DamageWidget)
	{
		m_DamageWidget = Cast<UDamageFont>(m_WidgetComp->GetUserWidgetObject());
	}
	if (m_DamageWidget)
	{
		bIsActive = true;
		m_LifeTime = 10.0f;

		SetActorHiddenInGame(false);
		SetActorTickEnabled(true);

		m_DamageWidget->SetDamageBP(DamageAmount, eDamageType);
		m_DamageWidget->ShowDamage(DamageAmount, Location);
	}
}


// Called every frame
void AUI_Actor::Tick(float DeltaTime)
{
	if (!bIsActive)
	{
		return;
	}
	Super::Tick(DeltaTime);

	
	if (!m_PlayerController.IsValid())
	{
		if (ULocalPlayer* LP = GetWorld()->GetFirstLocalPlayerFromController())
		{
			m_PlayerController = LP->PlayerController;
		}
	}
	
	if (m_PlayerController.IsValid())
	{
		const FVector CamLoc = m_PlayerController->PlayerCameraManager->GetCameraLocation();
		SetActorRotation((CamLoc - GetActorLocation()).Rotation());

		//const float RiseHeight = 120.f;
		//const float t01 = FMath::Clamp(m_ElapsedTime / m_LifeTime, 0.f, 1.f);
		//const float ease = FMath::InterpEaseOut(0.f, 1.f, t01, 2.0f); // 지수 2~3 추천
		//SetActorLocation(m_Position + FVector::UpVector * (RiseHeight * ease));
	}

	m_LifeTime -= DeltaTime;

	if (m_LifeTime < 0.0f)
	{
		bIsActive = false;
		OnFinished.Broadcast(this);
	}
}

