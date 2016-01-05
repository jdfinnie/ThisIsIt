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

}

void AWeapon::Tick(float DeltaTime)
{

	if (isReloading && reloadComplete)
	{
		ReloadComplete();
	}

}

void AWeapon::FireBegin()
{
	isFiring = true;

	
		switch (FireMode)
		{
		case FireMode::Single:
			//shoot once and consume input
			//Fire();
			GetWorldTimerManager().SetTimer(fireTimer, this, &AWeapon::Fire, WeaponInfo.TimeBetweenShots, true, false);
			//isFiring = false;
			break;
		case FireMode::Burst:
			//3 rounds then consume input
			GetWorldTimerManager().SetTimer(fireTimer, this, &AWeapon::Fire, WeaponInfo.TimeBetweenShots, true, false);
			break;
		case FireMode::Auto:
			// keep firing while ammo is availalbe and button is down
			//Fire();
			GetWorldTimerManager().SetTimer(fireTimer, this, &AWeapon::Fire, WeaponInfo.TimeBetweenShots, true, false);

			break;
		default:
			break;
		}
}

void AWeapon::FireEnd()
{
	isFiring = false;
	//if (FireMode == FireMode::Auto)
	//{
		GetWorld()->GetTimerManager().ClearTimer(fireTimer);
	//}
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
				Instant_Fire();
		}
		if (ProjectileType == ProjectileType::Spread)
		{
				for (int32 i = 0; i <= WeaponInfo.WeaponSpread; i++)
				{
					Instant_Fire();
				}
		}
		if (ProjectileType == ProjectileType::Projectile)
		{
				ProjectileFire();
		}

		PlayWeaponSound(FireSound);
		CurrentClip -= 1;
	}
	else
	{
		Reload();
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
	//CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		GetWorldTimerManager().SetTimer(reloadTimer, this, &AWeapon::ReloadComplete, 2, false, false);

}

void AWeapon::ReloadComplete() // this kinda sucks, i dont think its actually doing what I want
{
	//if (CurrentAmmo > 0)
	//{
		if (CurrentAmmo < WeaponInfo.MaxClip)
		{
			CurrentClip = CurrentAmmo;
		}
		else
		{
			CurrentAmmo -= WeaponInfo.MaxClip;
			CurrentClip += WeaponInfo.MaxClip;
		}

		GetWorld()->GetTimerManager().ClearTimer(reloadTimer);

		isReloading = false;
		reloadComplete = false;

		canFire = true;
	//}
	//else
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Blue, "NO AMMO");
	//}
}

void AWeapon::Instant_Fire()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = WeaponInfo.WeaponSpread;
	const float SpreadCone = FMath::DegreesToRadians(WeaponInfo.WeaponSpread * 0.5);

	FVector AimDir;

	//if this is the player, adjust to where the camera is looking
	AThisIsItCharacter *player = Cast<AThisIsItCharacter>(MyPawn);

	if (player)
		AimDir =  player->GetFollowCamera()->GetForwardVector();
	else
		AimDir = WeaponMesh->GetSocketRotation("Muzzle").Vector();

	const FVector StartTrace = WeaponMesh->GetSocketLocation("Muzzle");


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
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	//DrawDebugLine(this->GetWorld(), Origin, Impact.TraceEnd, FColor::Red, true, 10000, 10.f);

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

	AThisIsItCharacter *hitPlayer = Cast<AThisIsItCharacter>(Impact.GetActor());

	//wgho fired?
	AThisIsItCharacter *playerOwner = Cast<AThisIsItCharacter>(MyPawn);
	ABaseCharacter *NPCOwner = Cast<ABaseCharacter>(MyPawn);


	//was an NPC hit?
	if (hitNPC)
	{
		//hit by another npc?
		if (NPCOwner)
		{
			if (hitNPC->GetNPCType() != NPCOwner->GetNPCType())
			{
				hitNPC->CalculateHealth(-WeaponInfo.Damage);
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "YOU HIT AN ENEMY!!");
			}
		}
		
		//hit by player?
		if (playerOwner)
		{
			//not on the same team?
			if (hitNPC->GetNPCType() != NPCType::Human)
			{
				hitNPC->CalculateHealth(-WeaponInfo.Damage);
			}
		}
	}
	else if (hitPlayer)
	{
		//no firendly fire
		if (NPCOwner->GetNPCType() != NPCType::Human)
		{
			//dummy function while im here... implement eventually
			//playerOwner->TakeDamage(WeaponInfo.Damage);

			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Player Hit!");
		}

	}
}

void AWeapon::ProjectileFire()
{
	//this will launch an actual projectile like a rocket or the likes
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

