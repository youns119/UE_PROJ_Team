#include "Monster/WaveManager.h"
#include "Monster/SpawnerBase.h"
#include "Kismet/GameplayStatics.h"

bool UWaveManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (const UWorld* world = Cast<UWorld>(Outer))
	{
		if (!world->IsGameWorld())
		{
			return false;
		}

		return true;
	}
	return false;
}

void UWaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UWorld* world = GetWorld();
	if (!world || !world->IsGameWorld())
	{
		return;
	}

	//Using TimerHandle to delay
	FTimerHandle H;
	GetWorld()->GetTimerManager().SetTimer(H, this, &UWaveManager::Setting_SpawnerList, 1.f, false);

	UE_LOG(LogTemp, Warning, TEXT("WaveManager Initialized"));
}

void UWaveManager::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogTemp, Warning, TEXT("WaveManager Deinitialized"));
}

int32 UWaveManager::Get_Wave()
{
	return m_iWave + 1;
}

void UWaveManager::Set_Wave(int32 _iWave)
{
	//-1 : Next Wave
	if (_iWave < 0)
	{
		m_iWave++;
	}
	// or Specific Wave
	else
	{
		m_iWave = _iWave;
	}
	if (m_SpawnerList.IsValidIndex(m_iWave))
	{
		Start_Wave();
		m_iSpawnerCount = m_SpawnerList[m_iWave].SpawnerArray.Num();
	}

	UE_LOG(LogTemp, Warning, TEXT("m_iWave is  %d"), m_iWave);
}

void UWaveManager::AllMonsterClear()
{
	if (m_iSpawnerCount > 0)
	{
		return;
	}

	m_bAllMonsterClear = true; // 클리어조건
}

void UWaveManager::SpawningComplete()
{
	m_iSpawnerCount--;
}

void UWaveManager::Start_Wave()
{
	if (m_SpawnerList.IsValidIndex(m_iWave))
	{
		for (ASpawnerBase* Spawner : m_SpawnerList[m_iWave].SpawnerArray)
		{
			if (Spawner)
			{
				Spawner->BeginSpawning();
				UE_LOG(LogTemp, Warning, TEXT("Begin Spawning %d"), m_iWave);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Spawner for Wave %d"), m_iWave);
	}
}

int32 UWaveManager::Get_TagIndex(const TArray<FName>& _tagName)
{
	for (const FName& Tag : _tagName)
	{
		const FString str = Tag.ToString();
		if (str.Len() && FChar::IsDigit(str[str.Len() - 1]))
		{
			return (str[str.Len() - 1] - '0'); // 0~9
		}
	}
	return -1;
}

void UWaveManager::Setting_SpawnerList()
{
	m_SpawnerList.Reset();

	UWorld* world = GetWorld();
	if (!world || !world->IsGameWorld())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("WaveManager SettingSpawner"));

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(world, ASpawnerBase::StaticClass(), Found);

	UE_LOG(LogTemp, Warning, TEXT("FOUND Spawner %d"), Found.Num());

	int a = 0;

	for (AActor* iter : Found)
	{
		ASpawnerBase* Spawner = Cast<ASpawnerBase>(iter);
		if (!Spawner)
		{
			continue;
		}

		int32 iIndex = Get_TagIndex(Spawner->Tags);

		if (iIndex < 0)
		{
			continue;
		}

		if (m_SpawnerList.Num() <= iIndex)
		{
			m_SpawnerList.SetNum(iIndex + 1);
		}

		m_SpawnerList[iIndex].SpawnerArray.Add(Spawner);


		UE_LOG(LogTemp, Warning, TEXT("Spawner Adding! %d %d (%s)"),
			++a, iIndex, *GetNameSafe(Spawner));  // ← %d 자리에 숫자, %s 자리에 문자열
	}
}
