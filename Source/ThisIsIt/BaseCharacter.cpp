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
		
		if (!target)
		{
			state = defaultState;
			isAttacking = false;
		}
		else
		{
			if (!isAttacking && CanShoot())
				Attack();

			//the cast here may be redundant... test it out when you (James) have a chance
			ABaseCharacter *targ = Cast<ABaseCharacter>(target);
			if (targ)
			{
				if (targ->health <= 0)
				{
					target = NULL;
					state = defaultState;
					isAttacking = false;
					CurrentWeapon->FireEnd();
				}
			}
		}
		break;
	case State::Dead:
		//just in case
		isDead = true;
		if (isAttacking)
		{
			CurrentWeapon->FireEnd();
			CurrentWeapon->OnUnEquip();
			//CurrentWeapon->Destroy();
			CurrentWeapon->ConditionalBeginDestroy();
			isAttacking = false;
			GetCapsuleComponent()->SetActive(false);
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
	//something like this for percentage based damage mitigation... should be set at onCreate to avoif having to do this all the time (unless we plan to alter it) 
	//float mit = 100 - stats.armorValue;
	//float damage = delta * (mit / 100);
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::SanitizeFloat(damage));

	health -= delta;
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
		//CurrentWeapon->Destroy();
		CurrentWeapon->ConditionalBeginDestroy();
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
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Base Equip Default");
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
		CurrentWeapon->OnUnEquip();
		CurrentWeapon = Weapon;
		Weapon->SetOwningPawn(this);
		Weapon->OnEquip();
	}
	else
	{
		CurrentWeapon = Weapon;
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

//StopAttacking() - return player to default state and all that stuff

//there should be a stop attacking that returns to default state
void ABaseCharacter::Reload()
{
	isReloading = true;
	if (isAttacking)
	{
		isAttacking = false;
	}
}

//the animation blueprint calls this function when the reload anim finishes
void ABaseCharacter::ReloadComplete()
{
	isReloading = false;
	CurrentWeapon->ReloadComplete();
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