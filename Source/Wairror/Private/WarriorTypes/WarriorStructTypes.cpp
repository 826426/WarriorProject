// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorTypes/WarriorStructTypes.h"
#include "AbilitySystem/Abilities/WairrorGameplayAbility.h"

bool FWrriorHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
