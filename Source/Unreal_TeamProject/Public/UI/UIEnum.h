#pragma once

#include "CoreMinimal.h"
#include "UIEnum.generated.h"


UENUM(Blueprintable)
enum class EUIDamageType : uint8
{
	None UMETA(DisplayName = "None"),
	Normal UMETA(DisplayName = "Normal"),
	Fire UMETA(DisplayName = "Fire"),
	Lightning UMETA(DisplayName = "Lightning"),
	Chaos UMETA(DisplayName = "Chaos")
};
