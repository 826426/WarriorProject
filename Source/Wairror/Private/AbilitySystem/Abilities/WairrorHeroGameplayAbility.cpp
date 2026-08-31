// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WairrorHeroGameplayAbility.h"
#include "Characters/WairrorHeroCharacter.h"
#include "Controller/WairrorHeroController.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "WairroGamePlayerTags.h"

AWairrorHeroCharacter* UWairrorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
    if (!CachedWairrorHeroCharacter.IsValid()) {
        CachedWairrorHeroCharacter = Cast<AWairrorHeroCharacter>(CurrentActorInfo->AvatarActor);
    }

    return CachedWairrorHeroCharacter.IsValid() ? CachedWairrorHeroCharacter.Get() : nullptr;
}

AWairrorHeroController* UWairrorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
    if (!CachedWairrorHeroController.IsValid()) {
        CachedWairrorHeroController = Cast<AWairrorHeroController>(CurrentActorInfo->PlayerController);
    }

    return CachedWairrorHeroController.IsValid() ? CachedWairrorHeroController.Get() : nullptr;
}

UHeroConbatComponent* UWairrorHeroGameplayAbility::GetHeroConbatComponentFromActorInfo()
{

    return GetHeroCharacterFromActorInfo()->GetHeroConbatComponent();
}

FGameplayEffectSpecHandle UWairrorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount)
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
        InWeaponBaseDamage
    );

    if (InCurrentAttackTypeTag.IsValid()) {
        EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, InUsedComboCount);
    }
    return EffectSpecHandle;
}
