// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WarriorTypes/WarriorEnumType.h"
#include "WairrorBaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WAIRROR_API AWairrorBaseGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AWairrorBaseGameMode();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	EWarriorGameDifficulty CurrentGameDifficulty;


public:
	FORCEINLINE EWarriorGameDifficulty GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }
};
