// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_AttributeSet.h"

UPlayer_AttributeSet::UPlayer_AttributeSet()
{
    // 초기값 설정은 여기서도 가능하지만 보통 DataTable/GE로 초기화
}

void UPlayer_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetHealth_Max());
    }
}

void UPlayer_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        const float LocalDamageDone = GetDamage();
        SetDamage(0.f);

        if (LocalDamageDone > 0.f)
        {
            const float NewHealth = GetHealth() - LocalDamageDone;
            SetHealth(FMath::Clamp(NewHealth, 0.f, GetHealth_Max()));
        }
    }
}

void UPlayer_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayer_AttributeSet, Health, OldHealth);
}

void UPlayer_AttributeSet::OnRep_HealthMax(const FGameplayAttributeData& OldHealthMax)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayer_AttributeSet, Health_Max, OldHealthMax);
}

void UPlayer_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
}
