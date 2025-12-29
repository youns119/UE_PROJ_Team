#include "Effect/Data/FXData.h"
#include "NiagaraSystem.h"

UNiagaraSystem* UFXData::GetResolveSystem_NoBlock() const
{
	return Effect.Get();
}