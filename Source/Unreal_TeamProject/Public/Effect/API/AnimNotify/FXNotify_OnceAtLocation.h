#pragma once

#include "CoreMinimal.h"
#include "Effect/API/AnimNotify/FXNotify_Base.h"
#include "FXNotify_OnceAtLocation.generated.h"

UCLASS(meta = (DisplayName = "FX : Play Once AtLocation"))
class UNREAL_TEAMPROJECT_API UFXNotify_OnceAtLocation 
	: public UFXNotify_Base
{
	GENERATED_BODY()

public :
	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Anim) override;

private :
	UPROPERTY(EditAnywhere, Category = "FX|World")
	FFXWorldPlacement WorldPlacement{};
};