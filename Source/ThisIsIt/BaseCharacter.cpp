// Fill out your copyright notice in the Description page of Project Settings.

#include "ThisIsIt.h"
#include "Weapon.h"
#include "BaseCharacter.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentWeapon = NULL;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (type == Type::Robot)
	{
		state = State::Seeking;
		defaultState = State::Seeking;
	}
	else
	{
		state = State::Idle;
		defaultState = State::Idle;
	}

	GiveDefaultWeapon();
}

// Called every frame
void ABaseCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (state != State::Dead)
		CalculateDead();

	switch (state)
	{
	case State::Idle:
		break;
	case State::Seeking:
		break;
	case State::Attacking:

		
		if (!target) // change this to check for health before attacking? GetTargetHealth/State()?
		{
			state = State::Seeking;
			isAttacking = false;
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "I have no target");
		}
		else
		{
			if (!isAttacking && CanShoot())
				Attack();

			ABaseCharacter *targ = Cast<ABaseCharacter>(target);
			if (targ)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "target aquired");
				if (targ->health <= 0)
				{
					target = NULL;
					state = defaultState;
					isAttacking = false;
					CurrentWeapon->FireEnd();

					//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "target is dead. get outta here");
				}
			}
		}
		break;
	case State::Dead:
		//jus tin case
		isDead = true;
		if (isAttacking)
		{
			CurrentWeapon->FireEnd();
			CurrentWeapon->OnUnEquip();
			isAttacking = false;
		}
		break;
	default:
		break;
	}

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

//calculate health
void ABaseCharacter::CalculateHealth(float delta)
{
	float result = delta * ((100 - stats.armorValue) / 100);
	health += result;
	CalculateDead();
}
//calculate dead
void ABaseCharacter::CalculateDead()
{
	if (health <= 0)
	{
		//just go with it...
		state = State::Dead;
		isDead = true; 
	}
	else
	{
		isDead = false;
	}
}

//post-edit change property
#if WITH_EDITOR
void ABaseCharacter::PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent)
{
	isDead = false;
	health = 100;
	Super::PostEditChangeProperty(propertyChangedEvent);
	CalculateDead();
}
#endif

void ABaseCharacter::GiveDefaultWeapon()
{
	AWeapon *Spawner = GetWorld()->SpawnActor<AWeapon>(stats.primaryWeapon);
	if (Spawner)
	{
		EquipWeapon(Spawner);
	}
}

void ABaseCharacter::EquipWeapon(AWeapon *Weapon)
{
	if (CurrentWeapon != NULL)
	{
		//CurrentWeapon = Inventory[CurrentWeapon->WeaponConfig.Priority];
		CurrentWeapon->OnUnEquip();
		CurrentWeapon = Weapon;
		Weapon->SetOwningPawn(this);
		Weapon->OnEquip();
	}
	else
	{
		CurrentWeapon = Weapon;
		//CurrentWeapon = Inventory[CurrentWeapon->WeaponConfig.Priority];
		CurrentWeapon->SetOwningPawn(this);
		Weapon->OnEquip();
	}
}

void ABaseCharacter::Attack()
{
	if (target)
	{
		if (CurrentWeapon)
		{
			isAttacking = true;
			CurrentWeapon->FireBegin();
		}
	}
}

//there should be a stop attacking that returns to default state

void ABaseCharacter::ReloadComplete()
{
	isReloading = false;
	CurrentWeapon->ReloadComplete();
	//isReloading = false;
}

bool ABaseCharacter::CanShoot()
{
	if (isReloading || isJumping || isSwapingWeapons || isRunning)
		return false;

	return true;
}


Type ABaseCharacter::GetType()
{
	return type;
}