// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnConbatComponent.h"
#include "EnemyConbatComponent.generated.h"

/**
 * 
 */
UCLASS()
class WAIRROR_API UEnemyConbatComponent : public UPawnConbatComponent
{
	GENERATED_BODY()

public:
	virtual void OnHitTargetActor(AActor* HitActor) override;
	
};
