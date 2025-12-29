#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIEnum.h"
#include "DamageFont.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class UNREAL_TEAMPROJECT_API UDamageFont : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//UFUNCTION(BlueprintCallable, Category = "Damage") 이렇게 추천하긴함, 아마 Build.cs 에 추가해야할듯?
	UFUNCTION(BlueprintCallable, Category = "UMG")
	void ShowDamage(float DamageAmount, FVector Location); // Pool 에서 꺼내오거나 생성

	UFUNCTION(BlueprintCallable, Category = "UMG")
	void ReleaseToPool(); // Pool에 반환

	void Set_UIManager(class UUIManager* myUIManager);

	UFUNCTION(BlueprintImplementableEvent, Category = "UMG")
	void SetDamageBP(float NewValue, EUIDamageType eDamageType);

	UFUNCTION(BlueprintCallable, Category = "UMG")
	FLinearColor GetColor_DamageType(EUIDamageType eDamageType);

protected:
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* Anim_DamageFont = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* Anim_Turn = nullptr;
	
	//UFUNCTION()
	//void OnAnimationFinished(UWidgetAnimation* Animation);

private:
	UPROPERTY()
	TWeakObjectPtr<class UUIManager> m_UIManager; // UIManager에 대한 참조

	

};
