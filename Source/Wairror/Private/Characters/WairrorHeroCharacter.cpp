// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WairrorHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Dataassets/Input/DataAsset_InputConfig.h"
#include "Components/Input/WairrorInputComponent.h"
#include "WairroGamePlayerTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Dataassets/StartUpData/DataAsset_HeroStartUpData.h"
#include "Components/Combat/HeroConbatComponent.h"
#include "Components/UI/HeroUIComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "WairrorDebugHelper.h"

AWairrorHeroCharacter::AWairrorHeroCharacter() {

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.0f;
	CameraBoom->SocketOffset = FVector(0.f, 55.0f, 65.0f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	HeroConbatComponent = CreateDefaultSubobject<UHeroConbatComponent>(TEXT("HeroConbatComponent"));

	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
}

UPawnConbatComponent* AWairrorHeroCharacter::GetPawnConbatComponent() const
{
	return HeroConbatComponent;
}

UPwanUIComponent* AWairrorHeroCharacter::GetPwanUIComponent() const
{
	return HeroUIComponent;
}

UHeroUIComponent* AWairrorHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

void AWairrorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!CharacterStartUpData.IsNull()) {

		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous()) {
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
		}
	}
}

void AWairrorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset, TEXT("Forget to assgin a valid data asset as input config"));
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultInputMappingContext, 0);

	UWairrorInputComponent* WairrorInputComponent = CastChecked<UWairrorInputComponent>(PlayerInputComponent);

	WairrorInputComponent->BindNativeInputAction(InputConfigDataAsset, WairroGamePlayerTags::InputTag_Move, ETriggerEvent::Triggered, this, &AWairrorHeroCharacter::Input_Move);
	WairrorInputComponent->BindNativeInputAction(InputConfigDataAsset, WairroGamePlayerTags::InputTag_Look, ETriggerEvent::Triggered, this, &AWairrorHeroCharacter::Input_Look);

	WairrorInputComponent->BindNativeInputAction(InputConfigDataAsset, WairroGamePlayerTags::InputTag_SwitchTarget, ETriggerEvent::Triggered, this, &AWairrorHeroCharacter::Input_SwitchTargetTriggered);
	WairrorInputComponent->BindNativeInputAction(InputConfigDataAsset, WairroGamePlayerTags::InputTag_SwitchTarget, ETriggerEvent::Completed, this, &AWairrorHeroCharacter::Input_SwitchTargetCompleted);

	WairrorInputComponent->BindNativeInputAction(InputConfigDataAsset, WairroGamePlayerTags::InputTag_PickUp_Stones, ETriggerEvent::Started, this, &AWairrorHeroCharacter::Input_PickUpStonesStarted);
	WairrorInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &AWairrorHeroCharacter::Input_AbilityInputPressed, &AWairrorHeroCharacter::Input_AbilityInputReleased);
}


void AWairrorHeroCharacter::BeginPlay() {
	Super::BeginPlay();

}

void AWairrorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.F);

	if (MovementVector.Y != 0.f) {
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);

		AddMovementInput(ForwardDirection, MovementVector.Y);
	}

	if (MovementVector.X != 0.f) {
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AWairrorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	
	if (LookAxisVector.X != 0.f) {
		AddControllerYawInput(LookAxisVector.X);
	}
	if (LookAxisVector.Y != 0.f) {
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
void AWairrorHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	SwitchDirection = InputActionValue.Get<FVector2D>();
}
void AWairrorHeroCharacter::Input_SwitchTargetCompleted(const FInputActionValue & InputActionValue)
{
	FGameplayEventData Data;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		SwitchDirection.X > 0.0f ? WairroGamePlayerTags::Player_Event_SwitchTarget_Right : WairroGamePlayerTags::Player_Event_SwitchTarget_Left,
		Data
	);


}
void AWairrorHeroCharacter::Input_PickUpStonesStarted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		WairroGamePlayerTags::Player_Event_ConsumeStones,
		Data
	);
}
void AWairrorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag) 
{
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}
void AWairrorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}
