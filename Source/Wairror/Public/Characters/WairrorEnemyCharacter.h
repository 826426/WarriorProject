// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WairrorBaseCharacter.h"
#include "WairrorEnemyCharacter.generated.h"

class UEnemyConbatComponent;
class UEnemyUIComponent;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class WAIRROR_API AWairrorEnemyCharacter : public AWairrorBaseCharacter
{
	GENERATED_BODY()
	
public:
	AWairrorEnemyCharacter();

	//~Begin IPawnCombatInterface Interface
	virtual UPawnConbatComponent* GetPawnConbatComponent() const override;
	//~ End IPawnCombatInterface Interface

	//~Begin IPwanUIInterface Interface
	virtual UPwanUIComponent* GetPwanUIComponent() const override;
	virtual UEnemyUIComponent* GetEnemyUIComponent() const override;
	//~ End IPwanUIInterface Interface

protected:
	virtual void BeginPlay() override;


	//~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UEnemyConbatComponent* EnemyConbatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UEnemyUIComponent* EnemyUIComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* EnemyHealthWidgetComponent;

private:
	void InitEnemyStartUpData();

public:
	FORCEINLINE UEnemyConbatComponent* GetEnemyConbatComponent() const { return EnemyConbatComponent; }

};
