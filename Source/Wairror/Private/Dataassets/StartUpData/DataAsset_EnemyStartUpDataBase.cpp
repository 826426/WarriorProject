// Fill out your copyright notice in the Description page of Project Settings.


#include "Dataassets/StartUpData/DataAsset_EnemyStartUpDataBase.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WairrorEnemyGameplayAbility.h"


void UDataAsset_EnemyStartUpDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	if (!EnemyCombatAbilities.IsEmpty()) {
		for (const TSubclassOf<UWairrorEnemyGameplayAbility>& AbilityClass : EnemyCombatAbilities) {
			if (!AbilityClass) continue;

			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;

			InASCToGive->GiveAbility(AbilitySpec);
		}
	}
}
