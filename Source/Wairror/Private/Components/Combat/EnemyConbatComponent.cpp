// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/EnemyConbatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WairroGamePlayerTags.h"
#include "WarriorFunctionLibrary.h"

#include "WairrorDebugHelper.h"

void UEnemyConbatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor)) {
		return;
	}

	OverlappedActors.AddUnique(HitActor);


	//TODO::
	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitActor, WairroGamePlayerTags::Player_Status_Blocking);
	const bool bIsMyAttackUnblockable = false;

	if (bIsPlayerBlocking && !bIsMyAttackUnblockable) {
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;

	if (bIsValidBlock) {
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			WairroGamePlayerTags::Player_Event_SuccessfulBlock,
			EventData
		);
	}
	else {
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			WairroGamePlayerTags::Shared_Event_MeleeHit,
			EventData
		);
	}
}
