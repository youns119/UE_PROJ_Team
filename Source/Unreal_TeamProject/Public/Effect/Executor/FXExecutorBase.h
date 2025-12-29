#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Effect/Core/DataType/FXType.h"
#include "FXExecutorBase.generated.h"

class IFXService;
class UFXData;

UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class UNREAL_TEAMPROJECT_API UFXExecutorBase 
	: public UObject
{
	GENERATED_BODY()
	
public :
	// Details on Spawning FX - World
	virtual void Execute_World(IFXService& Service, const UFXData* Data, const FFXCueParams_World& Param) {}

	// Details on Spawning FX - Attached
	virtual void Execute_Attached(IFXService& Service, const UFXData* Data, const FFXCueParams_Attached& Param) {}
};