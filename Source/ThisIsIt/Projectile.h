// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseCharacter.h"
#include "Projectile.generated.h"

UCLASS()
class THISISIT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditDefaultsOnly)
		float Damage;

	UPROPERTY(EditDefaultsOnly)
		USoundCue* HitSound;

	UPROPERTY(EditAnywhere)
		UParticleSystem* HitEffect;

	// Sets default values for this actor's properties
	AProjectile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		UProjectileMovementComponent* ProjectileMovement;

	UFUNCTION()
		void OnCollision(AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	
};
