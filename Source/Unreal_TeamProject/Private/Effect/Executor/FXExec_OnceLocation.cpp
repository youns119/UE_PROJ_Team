#include "Effect/Executor/FXExec_OnceLocation.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Effect/Data/FXData.h"
#include "Effect/Core/Interface/IFXService.h"
#include "Effect/Util/FXUtil.h"

void UFXExec_OnceLocation::Execute_World(IFXService& Service, const UFXData* Data, const FFXCueParams_World& Param)
{
	if (!Data)
	{
		UE_LOG(LogFX, Log, TEXT("[%s] : No FXData on FX"), 
			*GetName());

		return;
	}

	UNiagaraSystem* FX = Data->GetResolveSystem_NoBlock();
	if (!FX)
	{
		UE_LOG(LogFX, Log, TEXT("[%s] : No Resolved FXData (%s)"),
			*GetName(), *Data->GetCueTag().ToString());

		return;
	}

	const float Scale = FXUtil::GetResolveScale(Data, Param.Scale);

	if (UNiagaraComponent* FXComponent = Service.SpawnOnceLocation(FX, Param.WorldTransform, Scale))
	{
		const float LifeTime = FXUtil::GetResolveLifeTime(Data, Param.LifeTimeOverride);
		if (LifeTime > 0)
		{
			Service.ReleaseAfter(FXComponent, LifeTime);
		}
	}
}