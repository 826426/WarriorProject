// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WairrorEnemyGameplayAbility.h"
#include "Characters/WairrorEnemyCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "WairroGamePlayerTags.h"

AWairrorEnemyCharacter* UWairrorEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
    if (!CachedWarriorEnemyCharacter.IsValid()) {
        CachedWarriorEnemyCharacter = Cast<AWairrorEnemyCharacter>(CurrentActorInfo->AvatarActor);
    }
    return CachedWarriorEnemyCharacter.IsValid() ? CachedWarriorEnemyCharacter.Get() : nullptr;
}

UEnemyConbatComponent* UWairrorEnemyGameplayAbility::GetEnemyConbatComponentFromActorInfo()
{
    return GetEnemyCharacterFromActorInfo()->GetEnemyConbatComponent();
}

FGameplayEffectSpecHandle UWairrorEnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat)
{
    check(EffectClass);

    FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
    ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
        EffectClass,
        GetAbilityLevel(),
        ContextHandle
    );

    EffectSpecHandle.Data->SetSetByCallerMagnitude(
        WairroGamePlayerTags::Shared_SetByCaller_BaseDamage,
        InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel())
    );

    return EffectSpecHandle;
}
