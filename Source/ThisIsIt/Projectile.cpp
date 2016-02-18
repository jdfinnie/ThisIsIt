// Fill out your copyright notice in the Description page of Project Settings.

#include "ThisIsIt.h"
#include "Projectile.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnCollision);
	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	InitialLifeSpan = 5.f;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AProjectile::OnCollision(AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		if (HitSound)
		{
			UAudioComponent* AC = NULL;

			AC = UGameplayStatics::SpawnSoundAtLocation(GetWorld(),HitSound,SweepResult.ImpactPoint);
		}


		if (HitEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, SweepResult.ImpactPoint);

		}

		ABaseCharacter *Shooter = Cast<ABaseCharacter>(Instigator);
		ABaseCharacter *Enemy = Cast<ABaseCharacter>(SweepResult.GetActor());
		if (Enemy && Shooter)
		{
			if (Enemy->GetType() != Shooter->GetType())
			{
				Enemy->CalculateHealth(Damage);
			}
		}
	}

	Destroy();
}

