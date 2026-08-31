// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WairrorGameplayAbility.h"
#include "WairrorEnemyGameplayAbility.generated.h"

class AWairrorEnemyCharacter;
class UEnemyConbatComponent;

/**
 * 
 */
UCLASS()
class WAIRROR_API UWairrorEnemyGameplayAbility : public UWairrorGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWairrorEnemyCharacter* GetEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UEnemyConbatComponent* GetEnemyConbatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat);

private:
	TWeakObjectPtr<AWairrorEnemyCharacter> CachedWarriorEnemyCharacter;

};
