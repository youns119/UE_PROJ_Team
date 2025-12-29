#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Prime_Helix.generated.h"

UENUM(BlueprintType)
enum class EPrimeHelixState : uint8
{
    FALL,
    SPAWN,
    LAND,
    IDLE,
    STUN,
    SUMMON,
    THROW_STONE,
    RISE_STONE,
    RUSH,
    END,
};

USTRUCT(BlueprintType)
struct FPrimeHelixAnims
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* FallMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* SpawnMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* IdleMontage;
};

UCLASS()
class UNREAL_TEAMPROJECT_API APrime_Helix : public ACharacter
{
    GENERATED_BODY()
public:
    APrime_Helix();

protected:
    virtual void BeginPlay() override;
    virtual void Landed(const FHitResult& Hit) override;


public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHealth = 100000.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackRange = 200.f;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite)
    //TSubclassOf<class APrime_Helix_Stone> StoneClass;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite)
    //TSubclassOf<class APrime_Helix_Minion> MinionClass;

    //UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    //TArray<APrime_Helix_Stone*> SpawnedStones;

    //UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    //TArray<APrime_Helix_Minion*> SpawnedMinions;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    ACharacter* TargetPlayer;

    UFUNCTION(BlueprintCallable)
    virtual void OnSpawned();

    UFUNCTION(BlueprintCallable)
    void DoThrowStone();

    UFUNCTION(BlueprintCallable)
    void DoPunch();

    UFUNCTION(BlueprintCallable)
    void DoSummonMinions();

    UFUNCTION(BlueprintCallable)
    void DoStoneRise();

    UFUNCTION(BlueprintCallable)
    void DoRush();

    //UFUNCTION(BlueprintCallable)
    //void OnStoneHit(APrime_Helix_Stone* Stone);

    UFUNCTION(BlueprintCallable)
    void SetStun(bool bStunned);

    // 애니메이션 연동
    UFUNCTION(BlueprintCallable)
    void PlayMontage(UAnimMontage* Montage);

public:
    EPrimeHelixState CurrentState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bHasLanded;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    FPrimeHelixAnims AnimMontages;
};