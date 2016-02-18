// Fill out your copyright notice in the Description page of Project Settings.

#include "ThisIsIt.h"
#include "Weapon.h"
#include "ThisIsItCharacter.h"
#include "Engine.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//set collision componenet
	//CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	//RootComponent = CollisionComp;

	//set weapon mesh
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	WeaponMesh->AttachTo(RootComponent);

	canFire = true;

	CurrentAmmo = WeaponInfo.MaxAmmo;
	CurrentClip = WeaponInfo.MaxClip;

}

void AWeapon::Tick(float DeltaTime)
{
	if (CurrentClip <= 0 && CurrentAmmo > 0)
	{
		Reload();
	}
}

void AWeapon::FireBegin()
{
	isFiring = true;

	//setting a timer to automatically fire again when the fire time has passed. cancels when button is released
	//good for player/automatic weapons. Great for controlling AI shooting
	GetWorldTimerManager().SetTimer(fireTimer, this, &AWeapon::Fire, WeaponInfo.TimeBetweenShots, true, false);
	
		//switch (FireMode)
		//{
		//case FireMode::Single:
		//	//shoot once and consume input
		//	GetWorldTimerManager().SetTimer(fireTimer, this, &AWeapon::Fire, WeaponInfo.TimeBetweenShots, true, false);
		//	break;
		//case FireMode::Burst:
		//	//3 rounds then consume input
		//	GetWorldTimerManager().SetTimer(fireTimer, this, &AWeapon::Fire, WeaponInfo.TimeBetweenShots, true, false);
		//	break;
		//case FireMode::Auto:
		//	// keep firing while ammo is availalbe and button is down
		//	GetWorldTimerManager().SetTimer(fireTimer, this, &AWeapon::Fire, WeaponInfo.TimeBetweenShots, true, false);

		//	break;
		//default:
		//	break;
		//}
}

void AWeapon::FireEnd()
{
	isFiring = false;

	GetWorld()->GetTimerManager().ClearTimer(fireTimer);

}

void AWeapon::Fire()
{
	if (CurrentClip > 0)
	{
		if (FireEffect)
		{

			UGameplayStatics::SpawnEmitterAttached
				(FireEffect,
				WeaponMesh,
				FName("Muzzle"),
				WeaponMesh->GetSocketLocation("Muzzle"),
				GetActorRotation(),
				EAttachLocation::KeepWorldPosition,
				true);
		}

		if (FireAnim)
		{
			WeaponMesh->PlayAnimation(FireAnim, false);
		}

		if (ProjectileType == ProjectileType::Bullet)
		{
				InstantFire();
		}
		else if (ProjectileType == ProjectileType::Spread)
		{
				for (int32 i = 0; i <= WeaponInfo.WeaponSpread; i++)
				{
					InstantFire();
				}
		}
		else if (ProjectileType == ProjectileType::Projectile)
		{
				ProjectileFire();
		}

		PlayWeaponSound(FireSound);
		CurrentClip -= 1;
	}
	else
	{
		//Reload();
	}
}

void AWeapon::SetOwningPawn(ACharacter * NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
	}
}

void AWeapon::OnEquip()
{
	CurrentAmmo = WeaponInfo.MaxAmmo;
	CurrentClip = WeaponInfo.MaxClip;

	AttachToPlayer();
}

void AWeapon::OnUnEquip()
{
	DetachFromPlayer();
}

void AWeapon::AttachToPlayer()
{
	if (MyPawn)
	{
		DetachFromPlayer();

		USkeletalMeshComponent *Character = MyPawn->GetMesh();
		WeaponMesh->SetHiddenInGame(false);
		WeaponMesh->AttachTo(Character, "RightHand");
	}
}

void AWeapon::DetachFromPlayer()
{
	WeaponMesh->DetachFromParent();
	WeaponMesh->SetHiddenInGame(true);
}

void AWeapon::Reload()
{
	if (CurrentAmmo > 0)
	{
		if (MyPawn)
		{
			ABaseCharacter *pawn = Cast<ABaseCharacter>(MyPawn);
			if (pawn)
			{
				pawn->Reload();
				canFire = false;
			}
		}
	}

}

void AWeapon::ReloadComplete() // this kinda sucks, i dont think its actually doing what I want
{
		if (CurrentAmmo < WeaponInfo.MaxClip)
		{
			CurrentClip = CurrentAmmo;
		}
		else
		{
			CurrentAmmo -= WeaponInfo.MaxClip;
			CurrentClip += WeaponInfo.MaxClip;
		}

		canFire = true;
}

void AWeapon::InstantFire()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = WeaponInfo.WeaponSpread;
	const float SpreadCone = FMath::DegreesToRadians(WeaponInfo.WeaponSpread * 0.5);

	FVector AimDir;
	FVector StartTrace;

	//if this is the player, adjust to where the camera is looking
	AThisIsItCharacter *player = Cast<AThisIsItCharacter>(MyPawn);

	if (player)
	{
		AimDir = player->GetFollowCamera()->GetForwardVector();
		StartTrace = player->GetFollowCamera()->GetComponentLocation();
	}
	else
	{
		AimDir = MyPawn->GetActorForwardVector();//WeaponMesh->GetSocketRotation("Muzzle").Vector();
		StartTrace = WeaponMesh->GetSocketLocation("Muzzle");
	}

	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, SpreadCone, SpreadCone);
	
	const FVector EndTrace = StartTrace + ShootDir * WeaponInfo.WeaponRange;

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);


}

FHitResult AWeapon::WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);

	FHitResult Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, TRACE_WEAPON, TraceParams);

	return Hit;
}

void AWeapon::ProcessInstantHit(const FHitResult &Impact, const FVector &Origin, const FVector &ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const FVector EndTrace = Origin + ShootDir * WeaponInfo.WeaponRange;
	 FVector EndPoint;
	
	 bool hit;

	if (Impact.GetActor())
	{
		EndPoint = Impact.ImpactPoint;
		hit = true;
	}
	else
	{
		EndPoint = EndTrace;
		hit = false;
	}

	if (hit)
	{
		if (HitEffect)
		{
			UGameplayStatics::SpawnEmitterAttached
				(HitEffect,
				RootComponent,
				NAME_None,
				EndPoint,
				GetActorRotation(),
				EAttachLocation::KeepWorldPosition,
				true);
		}

		//who got hit?
		ABaseCharacter *hitNPC = Cast<ABaseCharacter>(Impact.GetActor());

		//who fired?
		ABaseCharacter *NPCOwner = Cast<ABaseCharacter>(MyPawn);


		//was an NPC hit?
		if (hitNPC)
		{
			//hit by another npc?
			if (NPCOwner)
			{
				if (hitNPC->GetType() != NPCOwner->GetType())
				{
					hitNPC->CalculateHealth(WeaponInfo.Damage);
					//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "YOU HIT AN ENEMY!!");
				}
			}
		}
	}
}



void AWeapon::ProjectileFire()
{
	//this will launch an actual projectile like a rocket or the likes
	if (Projectile)
	{
		FRotator rot;

		//if this is the player, adjust the rotation to match the aim 
		AThisIsItCharacter *player = Cast<AThisIsItCharacter>(MyPawn);

		if (player)
		{
			rot = player->GetFollowCamera()->GetComponentRotation();
		}
		else
		{
			//rot = WeaponMesh->GetSocketRotation("Muzzle"); // horribly inaccurate
			rot = Instigator->GetActorRotation(); // this might make them a little too accurate
		}

		FVector pos = WeaponMesh->GetSocketLocation("Muzzle");

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		AProjectile *const proj = GetWorld()->SpawnActor<AProjectile>(Projectile, pos, rot, SpawnParams);
		if (proj)
		{

		}
	}
	else
	{
		//there is no projectile attached...
	}
}

UAudioComponent* AWeapon::PlayWeaponSound(USoundCue *Sound)
{
	UAudioComponent* AC = NULL;
	if (Sound && MyPawn)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, MyPawn->GetRootComponent());
	}

	return AC;
}

