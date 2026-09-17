// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/WairrorSurvialGameMode.h"
#include "Engine/AssetManager.h"
#include "Characters/WairrorEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"
#include "WarriorFunctionLibrary.h"

#include "WairrorDebugHelper.h"

void AWairrorSurvialGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	EWarriorGameDifficulty SavedGameDifficulty;

	if (UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(SavedGameDifficulty)) {
		CurrentGameDifficulty = SavedGameDifficulty;
	}
}

void AWairrorSurvialGameMode::BeginPlay()
{
	Super::BeginPlay();

	checkf(EnemyWaveSpawnerDataTable, TEXT("Forget to assign a vaild datat table in survial game mode blueprint"));

	SetCurrentSurvialGameModeState(EWairrorSurvialGameModeState::WaitSpawnNewWave);

	TotalWavesToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();

	PreLoadNextWaveEnemies();
}

void AWairrorSurvialGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSurvialGameModeState == EWairrorSurvialGameModeState::WaitSpawnNewWave) {
		TimePassedSinceStart += DeltaTime;

		if (TimePassedSinceStart >= SpawnNewWaveWaitTime) {
			TimePassedSinceStart = 0.0f;

			SetCurrentSurvialGameModeState(EWairrorSurvialGameModeState::SpawningNewWave);
		}
	}

	if (CurrentSurvialGameModeState == EWairrorSurvialGameModeState::SpawningNewWave) {
		TimePassedSinceStart += DeltaTime;

		if (TimePassedSinceStart >= SpawnEnemiesDelayTime) {

			CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();

			TimePassedSinceStart = 0.0f;

			SetCurrentSurvialGameModeState(EWairrorSurvialGameModeState::InProgress);
		}
	}

	if (CurrentSurvialGameModeState == EWairrorSurvialGameModeState::WaveCompleted) {
		TimePassedSinceStart += DeltaTime;

		if (TimePassedSinceStart >= WaveCompletedWaitTime) {


			TimePassedSinceStart = 0.0f;

			CurrentWaveCount++;

			if (HasFinishedAllWaves()) {
				SetCurrentSurvialGameModeState(EWairrorSurvialGameModeState::AllWaveDone);
			}
			else {
				SetCurrentSurvialGameModeState(EWairrorSurvialGameModeState::WaitSpawnNewWave);
				PreLoadNextWaveEnemies();
			}
		}
	}
}

void AWairrorSurvialGameMode::SetCurrentSurvialGameModeState(EWairrorSurvialGameModeState InState)
{
	CurrentSurvialGameModeState = InState;

	OnSurvialGameModeStateChanged.Broadcast(CurrentSurvialGameModeState);
}

bool AWairrorSurvialGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount > TotalWavesToSpawn;
}

void AWairrorSurvialGameMode::PreLoadNextWaveEnemies()
{
	if (HasFinishedAllWaves()) {
		return;
	}

	PreLoadedEnemyClassMap.Empty();

	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions) {
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(
				[SpawnerInfo, this]() {
					if (UClass* LoadedEnemyClass = SpawnerInfo.SoftEnemyClassToSpawn.Get()) {
						PreLoadedEnemyClassMap.Emplace(SpawnerInfo.SoftEnemyClassToSpawn, LoadedEnemyClass);
					}
				}
			)
		);
	}
}

FWarriorEnemyWaveSpawnerTableRow* AWairrorSurvialGameMode::GetCurrentWaveSpawnerTableRow() const
{
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));

	FWarriorEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FWarriorEnemyWaveSpawnerTableRow>(RowName, FString());

	checkf(FoundRow, TEXT("Could not find a valid row under the name %s in the data table"), *RowName.ToString());

	return FoundRow;
}

int32 AWairrorSurvialGameMode::TrySpawnWaveEnemies()
{
	if (TargetPointArray.IsEmpty()) {
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointArray);
	}

	checkf(!TargetPointArray.IsEmpty(), TEXT("No valid target point found in level: %s for spawning enemies"), *GetWorld()->GetName());

	uint32 EnemiesSpawnedThisTime = 0;

	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const FWarriorEnemyWaveSpawnerInfo SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions) {
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount, SpawnerInfo.MaxPerSpawnCount);

		UClass* LoadedEnemyClass = PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);

		for (int32 i = 0; i < NumToSpawn; i++) {
			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPointArray.Num() - 1);
			const FVector SpawnOrigin = TargetPointArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotation = TargetPointArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();

			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnOrigin, RandomLocation, 400.0f);

			RandomLocation += FVector(0.0f, 0.0f, 150.0f);

			AWairrorEnemyCharacter* SpawnEnemy = GetWorld()->SpawnActor<AWairrorEnemyCharacter>(LoadedEnemyClass, RandomLocation, SpawnRotation, SpawnParam);

			if (SpawnEnemy) {
				SpawnEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestyoed);

				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}

			if (!ShouldKeepSpawnEnemies()) {
				return EnemiesSpawnedThisTime;
			}
		}
	}

	return EnemiesSpawnedThisTime;
}

bool AWairrorSurvialGameMode::ShouldKeepSpawnEnemies() const
{
	return TotalSpawnedEnemiesThisWaveCounter < GetCurrentWaveSpawnerTableRow()->TotalEnemyToSpawnThisWave;
}

void AWairrorSurvialGameMode::OnEnemyDestyoed(AActor* DestroyedActor)
{
	CurrentSpawnedEnemiesCounter--;

	if (ShouldKeepSpawnEnemies()) {
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	else if (CurrentSpawnedEnemiesCounter == 0) {
		TotalSpawnedEnemiesThisWaveCounter = 0;
		CurrentSpawnedEnemiesCounter = 0;

		SetCurrentSurvialGameModeState(EWairrorSurvialGameModeState::WaveCompleted);
	}
}

void AWairrorSurvialGameMode::RegisterSpawnedEnemies(const TArray<AWairrorEnemyCharacter*>& InEnemiesToRegister)
{
	for (AWairrorEnemyCharacter* SpawnEnemy : InEnemiesToRegister) {
		if (SpawnEnemy) {
			CurrentSpawnedEnemiesCounter++;

			SpawnEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestyoed);
		}
	}
}
