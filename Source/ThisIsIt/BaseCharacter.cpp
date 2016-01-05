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

	if (npcType == NPCType::Robot)
	{
		npcState = NPCState::Seeking;
		defaultState = NPCState::Seeking;
	}
	else
	{
		npcState = NPCState::Idle;
		defaultState = NPCState::Idle;
	}

	GiveDefaultWeapon();
}

// Called every frame
void ABaseCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (npcState != NPCState::Dead)
		CalculateDead();

	switch (npcState)
	{
	case NPCState::Idle:
		break;
	case NPCState::Seeking:
		break;
	case NPCState::Attacking:

		
		if (!target) // change this to check for health before attacking? GetTargetHealth/State()?
		{
			npcState = NPCState::Seeking;
			isAttacking = false;
		}
		else
		{
			if (!isAttacking)
				Attack();

			ABaseCharacter *targ = Cast<ABaseCharacter>(target);
			if (targ)
			{
				if (targ->health <= 0)
				{
					target = NULL;
					npcState = defaultState;
						isAttacking = false;
						CurrentWeapon->FireEnd();
				}
			}
		}
		break;
	case NPCState::Dead:

		if (isAttacking)
			CurrentWeapon->FireEnd();

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
	health += delta;
	CalculateDead();
}
//calculate dead
void ABaseCharacter::CalculateDead()
{
	if (health <= 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("I should be dead now"));
		npcState = NPCState::Dead;

		isDead = true; // redundant now?
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
	AWeapon *Spawner = GetWorld()->SpawnActor<AWeapon>(Weapon);
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
		if (Weapon)
		{
			isAttacking = true;
			CurrentWeapon->FireBegin();
		}
	}
}

NPCType ABaseCharacter::GetNPCType()
{
	return npcType;
}