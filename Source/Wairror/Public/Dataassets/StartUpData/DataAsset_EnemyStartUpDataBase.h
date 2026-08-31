// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dataassets/StartUpData/DataAsset_StartUpDataBase.h"
#include "DataAsset_EnemyStartUpDataBase.generated.h"

class UWairrorEnemyGameplayAbility;

/**
 * 
 */
UCLASS()
class WAIRROR_API UDataAsset_EnemyStartUpDataBase : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UWairrorEnemyGameplayAbility>> EnemyCombatAbilities;
};
