// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ThisIsIt.h"
#include "ThisIsItCharacter.h"

//////////////////////////////////////////////////////////////////////////
// AThisIsItCharacter

AThisIsItCharacter::AThisIsItCharacter()
{

	CurrentWeapon = NULL;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Configure character movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); 
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SocketOffset = FVector(0, 25, 0); //move camera right (- for left)
	CameraBoom->TargetOffset = FVector(0, 0, 100); //Move camera up (- for down)
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->AttachTo(RootComponent);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation

	invert = -1;

	isRunning = false;
	isPrimaryEquipped = true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AThisIsItCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	//A-Button
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//triggers
	InputComponent->BindAction("RightTrigger", IE_Pressed, this, &AThisIsItCharacter::RightTriggerStart);
	InputComponent->BindAction("RightTrigger", IE_Released, this, &AThisIsItCharacter::RightTriggerStop);

	InputComponent->BindAction("LeftTrigger", IE_Pressed, this, &AThisIsItCharacter::LeftTriggerStart);
	InputComponent->BindAction("LeftTrigger", IE_Released, this, &AThisIsItCharacter::LeftTriggerStop);

	//shoulder buttons
	InputComponent->BindAction("RightBumper", IE_Pressed, this, &AThisIsItCharacter::RightBumperStart);
	InputComponent->BindAction("RightBumper", IE_Released, this, &AThisIsItCharacter::RightBumperStop);

	InputComponent->BindAction("LeftBumper", IE_Pressed, this, &AThisIsItCharacter::LeftBumper);
	//InputComponent->BindAction("LeftBumper", IE_Released, this, &ACharacter::StopJumping);

	//Y-Button
	InputComponent->BindAction("TopButton", IE_Pressed, this, &AThisIsItCharacter::TopButton);
	//InputComponent->BindAction("TopButton", IE_Released, this, &ACharacter::StopJumping);

	//B-Button
	InputComponent->BindAction("RightButton", IE_Pressed, this, &AThisIsItCharacter::RightButton);
	//InputComponent->BindAction("RightButton", IE_Released, this, &ACharacter::StopJumping);

	//X-Button
	InputComponent->BindAction("LeftButton", IE_Pressed, this, &AThisIsItCharacter::LeftButton);
	//InputComponent->BindAction("LeftButton", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Start", IE_Pressed, this, &AThisIsItCharacter::Start);
	InputComponent->BindAction("Select", IE_Pressed, this, &AThisIsItCharacter::Select);

	InputComponent->BindAxis("MoveForward", this, &AThisIsItCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AThisIsItCharacter::MoveRight);

	InputComponent->BindAction("LeftStick", IE_Pressed, this, &AThisIsItCharacter::RightBumperStart);
	InputComponent->BindAction("LeftStick", IE_Released, this, &AThisIsItCharacter::RightBumperStop);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AThisIsItCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AThisIsItCharacter::LookUpAtRate);
}

void AThisIsItCharacter::Start()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Start");
}

void AThisIsItCharacter::Select()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Select");
}

void AThisIsItCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AThisIsItCharacter::LookUpAtRate(float Rate)
{
	Rate *= invert; //this inverts the y for controllers and the likes

	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AThisIsItCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{	
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
	else if (Value == 0 && isRunning)
	{
		StopRunning();
	}
}

void AThisIsItCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		//AddMovementInput(GetActorRightVector(), Value);
		const FRotator Rotation = GetActorRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value / 2);
	}
}


void AThisIsItCharacter::BeginPlay()
{
	Super::BeginPlay();
	//SwitchWeapons();
}

void AThisIsItCharacter::Tick(float DeltaSeconds)
{
	//this is here to override the base tick which can cause problems

	if (state != State::Dead)
		CalculateDead();

	switch (state)
	{
	case State::Idle:
		break;
	case State::Seeking:
		break;
	case State::Attacking:
		break;
	case State::Dead:
		//just in case
		isDead = true;
		isAttacking = false;

		CurrentWeapon->FireEnd();

		GetCapsuleComponent()->SetActive(false);

		break;
	default:
		break;
	}
}

void AThisIsItCharacter::GiveDefaultWeapon()
{
	AWeapon *Spawner = GetWorld()->SpawnActor<AWeapon>(stats.primaryWeapon);
	if (Spawner)
	{
		EquipWeapon(Spawner);
		isPrimaryEquipped = true;
	}
}

void AThisIsItCharacter::EquipWeapon(AWeapon *Weapon)
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


void AThisIsItCharacter::SwitchWeapons()
{
		//is primary equipped?
		if (isPrimaryEquipped)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "switch to secondary");
			AWeapon *Spawner = GetWorld()->SpawnActor<AWeapon>(stats.secondaryWeapon);
			if (Spawner)
			{
				EquipWeapon(Spawner);
				isPrimaryEquipped = false;
			}
		}
		else
		{
			AWeapon *Spawner = GetWorld()->SpawnActor<AWeapon>(stats.primaryWeapon);
			if (Spawner)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "switch to primary");
				EquipWeapon(Spawner);
				isPrimaryEquipped = true;
			}
		}
}

//it might be better to seperate these out into a different class and leave this just to handle input...
void AThisIsItCharacter::RightTriggerStart()
{

	if (CurrentWeapon != NULL)
	{
		if (!isAttacking)
		{
			isAttacking = true;

			if (CanShoot())
				CurrentWeapon->FireBegin();

			if (isRunning)
			{
				StopRunning();
			}

		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "current weapon is null");
	}
}

void AThisIsItCharacter::RightTriggerStop()
{
	if (CurrentWeapon != NULL)
	{
		if (isAttacking)
		{
			isAttacking = false;
			CurrentWeapon->FireEnd();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "current weapon is null");
	}
}

void AThisIsItCharacter::LeftTriggerStart()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Aiming!"));
	isAiming = true;
	FollowCamera->FieldOfView = 60;

	if (isRunning)
	{
		StopRunning();
	}
}

void AThisIsItCharacter::LeftTriggerStop()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("not aiming"));
	isAiming = false;
	FollowCamera->FieldOfView = 90;
}

void AThisIsItCharacter::RightBumperStart()
{

	if (!isRunning && !isAiming && !isAttacking && !isReloading)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1200;
		isRunning = true;
	}
	else
	{
		StopRunning();
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Run?"));
}

void AThisIsItCharacter::StopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = 600;
	isRunning = false;
}

void AThisIsItCharacter::StopActions()
{
	StopRunning();

	//StopAiming()
	isAiming = false;
	FollowCamera->FieldOfView = 90;

	//StopAttacking()
	isAttacking = false;
	CurrentWeapon->FireEnd();
}

void AThisIsItCharacter::RightBumperStop()
{
	//GetCharacterMovement()->MaxWalkSpeed = 600;
	//isRunning = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Run?"));
}
void AThisIsItCharacter::LeftBumper()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Left Bumper!"));
}

void AThisIsItCharacter::TopButton()
{
	//just for now. should be moved to options
	//invert *= -1;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Inverting Y"));
}

void AThisIsItCharacter::RightButton()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Right Button!"));
	if (!isReloading && !isAttacking)
		SwitchWeapons();
}

void AThisIsItCharacter::LeftButton()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Reloading!"));
	if (!isReloading)
	{
		Reload();
	}
}