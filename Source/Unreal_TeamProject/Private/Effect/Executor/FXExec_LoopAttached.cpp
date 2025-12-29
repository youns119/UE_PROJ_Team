#include "Effect/Executor/FXExec_LoopAttached.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Effect/Data/FXData.h"
#include "Effect/Core/Interface/IFXService.h"
#include "Effect/Util/FXUtil.h"

void UFXExec_LoopAttached::Execute_Attached(IFXService& Service, const UFXData* Data, const FFXCueParams_Attached& Param)
{
	if (!Data || !Param.AttachTarget)
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

	USceneComponent* Target = Param.AttachTarget.Get();
	if (!IsValid(Target))
	{
		AActor* OwnerActor = Param.Owner.IsValid() ? Param.Owner->GetOwner() : nullptr;
		Target = FXUtil::GetResolveAttachTarget(OwnerActor, nullptr, Param.AttachInfo.ComponentRef, true);
	}

	if (!Target)
	{
		UE_LOG(LogFX, Log, TEXT("[%s] : No AttachTarget on FX"),
			*GetName());

		return;
	}

	const float Scale = FXUtil::GetResolveScale(Data, Param.Scale);
	const FName SocketName = FXUtil::GetResolveSocketName(Param.AttachInfo.SocketName);
	const FGameplayTag Tag = Data->GetCueTag();

	bool bReused{ false }, bReplaced{ false };

	UNiagaraComponent* FXComponent = Service.SpawnLoopAttached
	(
		FX, Tag, Param.AttachTarget, SocketName,
		Param.AttachInfo.RelativeOffset, Scale,
		bReused, bReplaced, Param.Owner.Get()
	);

	if (FXComponent)
	{
		if (Param.bRestart && bReused && !bReplaced)
		{
			FXComponent->ResetSystem();
		}

		const float LifeTime = FXUtil::GetResolveLifeTime(Data, Param.LifeTimeOverride);
		if (LifeTime > 0)
		{
			Service.ReleaseAfter(FXComponent, LifeTime);
		}
	}
}