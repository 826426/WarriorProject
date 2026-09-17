// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WarriorProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WarriorFunctionLibrary.h"
#include "WairroGamePlayerTags.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "WairrorDebugHelper.h"
// Sets default values
AWarriorProjectileBase::AWarriorProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AWarriorProjectileBase::OnProjectileBeginOverlap);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 700.0f;
	ProjectileMovementComponent->MaxSpeed = 900.0f;
	ProjectileMovementComponent->Velocity = FVector(1.0f, 0.0f, 0.0f);
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = 4.0f;
}

// Called when the game starts or when spawned
void AWarriorProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap) {
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

void AWarriorProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

	APawn* HitPawn = Cast<APawn>(OtherActor);

	if (!HitPawn || !UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn)) {
		Destroy();
		return;
	}

	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitPawn, WairroGamePlayerTags::Player_Status_Blocking);

	if (bIsPlayerBlocking) {
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(this, HitPawn);
	}

	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;

	if (bIsValidBlock) {
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitPawn,
			WairroGamePlayerTags::Player_Event_SuccessfulBlock,
			Data
		);
	}
	else {
		HandleApplyProjectileDamage(HitPawn, Data);
	}

	Destroy();
}

void AWarriorProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedActors.Contains(OtherActor)) {
		return;
	}
	OverlappedActors.AddUnique(OtherActor);


	if (APawn* HitPawn = Cast<APawn>(OtherActor)) {

		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;

		if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn)) {
			HandleApplyProjectileDamage(HitPawn, Data);
		}
	}
}

void AWarriorProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	checkf(ProjectileDamageEffectSpecHandle.IsValid(), TEXT("Forget to assgin a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());
	
	const bool bWasApplied = UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);
	
	if (bWasApplied) {
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			WairroGamePlayerTags::Shared_Event_HitReact,
			InPayload
		);
	}
}


