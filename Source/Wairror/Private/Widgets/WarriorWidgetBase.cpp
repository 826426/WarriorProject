// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/WarriorWidgetBase.h"
#include "Interface/PwanUIInterface.h"

void UWarriorWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IPwanUIInterface* PwanUIInterface = Cast<IPwanUIInterface>(GetOwningPlayerPawn())) {
		if (UHeroUIComponent* HeroUIComponent = PwanUIInterface->GetHeroUIComponent()) {

			BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);

		}
	}
}

void UWarriorWidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
	if (IPwanUIInterface* PwanUIInterface = Cast<IPwanUIInterface>(OwningEnemyActor)) {
		UEnemyUIComponent* EnemyUIComponent = PwanUIInterface->GetEnemyUIComponent();

		checkf(EnemyUIComponent, TEXT("Failed to extrac an EnemyUIComponent from %s"), *OwningEnemyActor->GetActorNameOrLabel());

		BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
	}
}
