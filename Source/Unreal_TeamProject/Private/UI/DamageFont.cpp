#include "UI/DamageFont.h"
#include "UI/UIManager.h"
#include "Kismet/GameplayStatics.h"

void UDamageFont::ShowDamage(float DamageAmount, FVector Location)
{
    if (Anim_DamageFont)
    {
        StopAnimation(Anim_DamageFont);
		StopAnimation(Anim_Turn);

        PlayAnimation(Anim_DamageFont, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);

        if (Anim_Turn && FMath::FRand() < 0.10f)
        {
			PlayAnimation(Anim_Turn, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
        }

        //UE_LOG(LogTemp, Log, TEXT("DamageFont ShowDamage called! Location: %s"), *Location.ToString());

		EUIDamageType myDamageType = EUIDamageType::Normal;
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("Anim_DamageFont is not bound. Check name/binding."));
    }
}

void UDamageFont::ReleaseToPool()
{
	//UE_LOG(LogTemp, Log, TEXT("ReleaseToPool Called"));
}

void UDamageFont::Set_UIManager(class UUIManager* myUIManager)
{
	//UE_LOG(LogTemp, Log, TEXT("Set_UIManager called!"));
	m_UIManager = myUIManager;
	//Á¦¹ß
}

FLinearColor UDamageFont::GetColor_DamageType(EUIDamageType eDamageType)
{
    switch (eDamageType)
    {
        case EUIDamageType::None:

            return FLinearColor(0.9f, 0.9f, 0.9f, 1.f);
        
		case EUIDamageType::Normal:

            return FLinearColor(0.9f, 0.9f, 0.9f, 1.f);

		case EUIDamageType::Fire:

            return FLinearColor(1.0f, 0.3f, 0.05f, 1.f);

		case EUIDamageType::Lightning:
            
            return FLinearColor(0.35f, 0.7f, 1.0f, 1.f);

        case EUIDamageType::Chaos:
            
            return FLinearColor(0.7f, 0.2f, 0.9f, 1.f);

        default:

            return FLinearColor(0.9f, 0.9f, 0.9f, 1.f);
    }

}