#pragma once

#include "CoreMinimal.h"
#include "Effect/Executor/FXExecutorBase.h"
#include "FXExec_OnceLocation.generated.h"

UCLASS()
class UNREAL_TEAMPROJECT_API UFXExec_OnceLocation 
	: public UFXExecutorBase
{
	GENERATED_BODY()

public :
	virtual void Execute_World(IFXService& Service, const UFXData* Data, const FFXCueParams_World& Param) override;
};