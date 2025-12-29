#include "Monster/Prime_Helix.h"
//#include "Prime_Helix_Stone.h"
//#include "Prime_Helix_Minion.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

APrime_Helix::APrime_Helix()
{
    CurrentHealth = MaxHealth;
    PrimaryActorTick.bCanEverTick = true;
    CurrentState = EPrimeHelixState::FALL;

    GetCharacterMovement()->GravityScale = 2.f;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Create Bosssssssssssssssssssss@@@"));

}

void APrime_Helix::BeginPlay()
{
    Super::BeginPlay();

    TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    PlayMontage(AnimMontages.FallMontage);
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("fall Called Montage@@@@@@@@@@@@@@@@"));

}

void APrime_Helix::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    if (CurrentState == EPrimeHelixState::FALL)
    {
        CurrentState = EPrimeHelixState::SPAWN;
        PlayMontage(AnimMontages.SpawnMontage);
        OnSpawned();
    }
}


void APrime_Helix::OnSpawned()
{
}

void APrime_Helix::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!TargetPlayer) return;

    float Distance = FVector::Distance(TargetPlayer->GetActorLocation(), GetActorLocation());

    if (Distance > AttackRange)
    {
        DoThrowStone();
    }
    else
    {
        DoPunch();
    }
}

void APrime_Helix::DoThrowStone()
{
    //for (int i = 0; i < 5; i++)
    //{
    //    FTimerHandle Timer;
    //    GetWorldTimerManager().SetTimer(Timer, [this]()
    //        {
    //            FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 200.f + FVector(0, 0, 50.f);
    //            APrime_Helix_Stone* Stone = GetWorld()->SpawnActor<APrime_Helix_Stone>(StoneClass, SpawnLoc, FRotator::ZeroRotator);
    //            if (Stone) SpawnedStones.Add(Stone);
    //        }, i * 1.f, false);
    //}
}

void APrime_Helix::DoPunch()
{
    //PlayMontage(PunchMontage);
}

void APrime_Helix::DoSummonMinions()
{
    //FVector Forward = GetActorForwardVector();
    //for (int i = 0; i < 4; i++)
    //{
    //    FVector SpawnLoc = GetActorLocation() + Forward * (200.f * (i + 1));
    //    APrime_Helix_Minion* Minion = GetWorld()->SpawnActor<APrime_Helix_Minion>(MinionClass, SpawnLoc, FRotator::ZeroRotator);
    //    if (Minion) SpawnedMinions.Add(Minion);
    //}
}

void APrime_Helix::DoStoneRise()
{
    //for (int i = 0; i < 20; i++)
    //{
    //    FVector RandomOffset = FVector(FMath::FRandRange(-500, 500), FMath::FRandRange(-500, 500), 0);
    //    FVector SpawnLoc = GetActorLocation() + RandomOffset + FVector(0, 0, -50);
    //    APrime_Helix_Stone* Stone = GetWorld()->SpawnActor<APrime_Helix_Stone>(StoneClass, SpawnLoc, FRotator::ZeroRotator);
    //    if (Stone) SpawnedStones.Add(Stone);
    //    Stone->RiseFromGround(); // Stone 내부 구현
    //}
}

void APrime_Helix::DoRush()
{
    if (!TargetPlayer) return;
    FVector Dir = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    GetCharacterMovement()->BrakingFrictionFactor = 0.f;
    LaunchCharacter(Dir * 2000.f + FVector(0, 0, 200.f), true, true);
}

//void APrime_Helix::OnStoneHit(APrime_Helix_Stone* Stone)
//{
//    SetStun(true);
//}

void APrime_Helix::SetStun(bool bStunned)
{
    if (bStunned)
    {
        //PlayMontage(StunMontage);
    }
    else
    {
        // Stun 해제 후 Idle로
    }
}

void APrime_Helix::PlayMontage(UAnimMontage* Montage)
{
    if (Montage)
    {
        PlayAnimMontage(Montage);

        if (!GetMesh())
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No SKeletal Mesh!"));
        }

        if (!GetMesh()->GetAnimInstance())
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No AnimBluePrint!"));
        }

    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Not Montage Calllllllllllllllllllled"));
    }
}