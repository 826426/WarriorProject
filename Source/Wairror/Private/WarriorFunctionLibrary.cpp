 // Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorFunctionLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interface/PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "WairroGamePlayerTags.h"

#include "WairrorDebugHelper.h"
UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	check(InActor);

	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	if (!ASC->HasMatchingGameplayTag(TagToAdd)) {
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor * InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	if (ASC->HasMatchingGameplayTag(TagToRemove)) {
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UWarriorFunctionLibrary::BP_DoesHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EWarriorConfirmType::Yes : EWarriorConfirmType::No;
}

UPawnConbatComponent* UWarriorFunctionLibrary::NativeGetPawnConbatComponentFromActor(AActor* InActor)
{
	check(InActor) {
		if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor)) {
			return PawnCombatInterface->GetPawnConbatComponent();
		}
	}
	return nullptr;
}

UPawnConbatComponent* UWarriorFunctionLibrary::BP_GetPawnConbatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType)
{
	UPawnConbatComponent* CombatComponent = NativeGetPawnConbatComponentFromActor(InActor);

	OutValidType = CombatComponent ? EWarriorValidType::Valid : EWarriorValidType::InValid;

	return CombatComponent;
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);

	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent) {
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}
	return false;
}

float UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* Invictim, float& OutAngleDifference)
{
	check(InAttacker && Invictim);

	const FVector VictimForward = Invictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalize = (InAttacker->GetActorLocation() - Invictim->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalize);

	OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalize);

	if (CrossResult.Z < 0.0f) {
		OutAngleDifference *= -1.0f;
	}

	if (OutAngleDifference >= -45.0f || OutAngleDifference <= 45.0f) {
		return WairroGamePlayerTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngleDifference < -45.0f || OutAngleDifference >= 135.0f) {
		return WairroGamePlayerTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngleDifference < -135.0f || OutAngleDifference > 135.0f) {
		return WairroGamePlayerTags::Shared_Status_HitReact_Back;
	}
	else if (OutAngleDifference > 45.0f || OutAngleDifference <= 135.0f) {
		return WairroGamePlayerTags::Shared_Status_HitReact_Right;
	}
	return WairroGamePlayerTags::Shared_Status_HitReact_Front;
}

bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDecfender)
{
	check(InAttacker && InDecfender);

	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDecfender->GetActorForwardVector());
	
	return DotResult < -0.1f;
}

