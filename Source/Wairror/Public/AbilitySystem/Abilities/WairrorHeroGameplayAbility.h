// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WairrorGameplayAbility.h"
#include "WairrorHeroGameplayAbility.generated.h"

class AWairrorHeroCharacter;
class AWairrorHeroController;
class UHeroConbatComponent;
class UHeroUIComponent;

/**
 * 
 */
UCLASS()
class WAIRROR_API UWairrorHeroGameplayAbility : public UWairrorGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWairrorHeroCharacter* GetHeroCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWairrorHeroController* GetHeroControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UHeroConbatComponent* GetHeroConbatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UHeroUIComponent* GetHeroUIComponenttFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	bool GetAbilityRemainingCooldownByTag(FGameplayTag InCoolDownTag, float& TotalCooldownTime, float& RemainingCooldownTime);

private:
	TWeakObjectPtr<AWairrorHeroCharacter> CachedWairrorHeroCharacter;
	TWeakObjectPtr<AWairrorHeroController> CachedWairrorHeroController;
};
