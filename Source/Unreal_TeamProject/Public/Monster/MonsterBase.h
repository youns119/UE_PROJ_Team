#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterBase.generated.h"

UCLASS(Blueprintable)
class UNREAL_TEAMPROJECT_API AMonsterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterBase();

protected:
	virtual void BeginPlay() override;

public:	
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Monster")
    void TakeDamageSimple(float DamageAmount);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Monster")
    void Attack();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Monster")
    void Die();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CurHP = 100.f;
};


//UPROPERTY             → 언리얼 시스템에서 노출시키는 변수
//UFUNCTION             → 언리얼 시스템에서 노출시키는 함수
// 
//EditAnywhere          → 에디터에서 조정 가능
//BlueprintReadWrite    → 블루프린트에서 읽고 쓰기 가능
//Category = "Stats"    → 에디터에서 그룹화

//BlueprintImplementableEvent   → 블루프린트 전용 구현
//BlueprintNativeEvent          → C++ 기본 구현 + 블루프린트 오버라이드 가능
