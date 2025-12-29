#pragma once

#include "CoreMinimal.h"
#include "Effect/Executor/FXExecutorBase.h"
#include "FXExec_OnceAttached.generated.h"

UCLASS()
class UNREAL_TEAMPROJECT_API UFXExec_OnceAttached 
	: public UFXExecutorBase
{
	GENERATED_BODY()
	
public :
	virtual void Execute_Attached(IFXService& Service, const UFXData* Data, const FFXCueParams_Attached& Param) override;
};