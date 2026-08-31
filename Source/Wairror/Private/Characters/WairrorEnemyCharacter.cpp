// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WairrorEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/EnemyConbatComponent.h"
#include "Engine/AssetManager.h"
#include "Dataassets/StartUpData/DataAsset_EnemyStartUpDataBase.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/WarriorWidgetBase.h"

#include "WairrorDebugHelper.h"

AWairrorEnemyCharacter::AWairrorEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.0f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.0f;

	EnemyConbatComponent = CreateDefaultSubobject<UEnemyConbatComponent>("EnemyCombatComponent");

	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>("EnemyUIComponent");

	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyHealthWidgetComponent");
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());
}

UPawnConbatComponent* AWairrorEnemyCharacter::GetPawnConbatComponent() const
{
	return EnemyConbatComponent;
}

UPwanUIComponent* AWairrorEnemyCharacter::GetPwanUIComponent() const
{
	return EnemyUIComponent;
}

UEnemyUIComponent* AWairrorEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

void AWairrorEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UWarriorWidgetBase* HealthWidget = Cast<UWarriorWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject())) {
		HealthWidget->InitEnemyCreatedWidget(this);
	}
}

void AWairrorEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitEnemyStartUpData();
}

void AWairrorEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull()) return;

	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get()) {
					LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);

				}
			}
		)
	);
}
