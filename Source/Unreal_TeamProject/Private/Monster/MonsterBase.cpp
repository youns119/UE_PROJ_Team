#include "Monster/MonsterBase.h"

AMonsterBase::AMonsterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterBase::TakeDamageSimple_Implementation(float DamageAmount)
{
    UE_LOG(LogTemp, Warning, TEXT("In DamageFunc : %f"), CurHP);

    if (DamageAmount > 0.f)
    {
        CurHP -= DamageAmount;
        UE_LOG(LogTemp, Warning, TEXT("Current HP : %f"), CurHP);

        if (CurHP <= 0.f)
        {
            UE_LOG(LogTemp, Warning, TEXT("0!!!!!!!!!!!!!"));

            CurHP = 0.f;
        }
    }
}

void AMonsterBase::Attack_Implementation()
{
}

void AMonsterBase::Die_Implementation()
{
}