// Fill out your copyright notice in the Description page of Project Settings.

#include "ThisIsIt.h"
#include "EditorPawn.h"

//////////////////////////////////////////////////////////////////////////
// AEditorPawn

AEditorPawn::AEditorPawn()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Configure character movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;


	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->FieldOfView = 55;

	GetCharacterMovement()->MaxWalkSpeed = 40000000;
	invert = -1;

	stepDistance = 1000;

	menuOpen = false;

	FString small = "Small";
	Buttons.Add(small);

	FString med = "Medium";
	Buttons.Add(med);

	FString large = "Large";
	Buttons.Add(large);

	CurrentButton = 0;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AEditorPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);

	//triggers
	InputComponent->BindAction("RightTrigger", IE_Pressed, this, &AEditorPawn::RightTriggerStart);
	InputComponent->BindAction("RightTrigger", IE_Released, this, &AEditorPawn::RightTriggerStop);

	InputComponent->BindAction("LeftTrigger", IE_Pressed, this, &AEditorPawn::LeftTriggerStart);
	InputComponent->BindAction("LeftTrigger", IE_Released, this, &AEditorPawn::LeftTriggerStop);

	//shoulder buttons
	InputComponent->BindAction("RightBumper", IE_Pressed, this, &AEditorPawn::RightBumperStart);
	//InputComponent->BindAction("RightBumper", IE_Released, this, &AEditorPawn::RightBumperStop);

	InputComponent->BindAction("LeftBumper", IE_Pressed, this, &AEditorPawn::LeftBumper);
	//InputComponent->BindAction("LeftBumper", IE_Released, this, &ACharacter::StopJumping);

	//Y-Button
	InputComponent->BindAction("TopButton", IE_Pressed, this, &AEditorPawn::TopButton);
	//InputComponent->BindAction("TopButton", IE_Released, this, &ACharacter::StopJumping);

	//B-Button
	InputComponent->BindAction("RightButton", IE_Pressed, this, &AEditorPawn::RightButton);
	//InputComponent->BindAction("RightButton", IE_Released, this, &ACharacter::StopJumping);

	//X-Button
	InputComponent->BindAction("LeftButton", IE_Pressed, this, &AEditorPawn::LeftButton);
	//InputComponent->BindAction("LeftButton", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Start", IE_Pressed, this, &AEditorPawn::Start);
	InputComponent->BindAction("Select", IE_Pressed, this, &AEditorPawn::Select);

	//A-Button
	InputComponent->BindAction("Jump", IE_Pressed, this, &AEditorPawn::BottomButton);

	InputComponent->BindAxis("MoveForward", this, &AEditorPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AEditorPawn::MoveRight);
}

void AEditorPawn::Start()
{

}

void AEditorPawn::Select()
{

}

void AEditorPawn::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AEditorPawn::LookUpAtRate(float Rate)
{
	//Rate *= invert; //this inverts the y for controllers and the likes

	// calculate delta for this frame from the rate information
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AEditorPawn::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (canMove)
		{
			FVector move = RootComponent->GetRelativeTransform().GetLocation();

			if (Value > 0)
			{
				if (move.X < 1000)
				{
					move.X += stepDistance;
				}
			}
			else
			{
				if (move.X > -29000)
				{
					move.X -= stepDistance;
				}
			}

			RootComponent->SetWorldLocation(move);

			canMove = false;
			GetWorldTimerManager().SetTimer(moveTimer, this, &AEditorPawn::ResetMove, 0.2f, false);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Wait");
		}
	}
}

void AEditorPawn::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (canMove)
		{
			FVector move = RootComponent->GetRelativeTransform().GetLocation();

			if (Value < 0)
			{
				if (move.Y > -1000)
				{
					move.Y -= stepDistance;
				}
			}
			else
			{
				if (move.Y < 29000)
				{
					move.Y += stepDistance;
				}
			}

			RootComponent->SetWorldLocation(move);

			canMove = false;
			GetWorldTimerManager().SetTimer(moveTimer, this, &AEditorPawn::ResetMove, 0.2f, false);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Wait");
		}
	}

}


void AEditorPawn::BeginPlay()
{
	Super::BeginPlay();
	canMove = true;
}

void AEditorPawn::RightTriggerStart()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Move slower");
	stepDistance /= 2;
}

void AEditorPawn::RightTriggerStop()
{
	//stop doing those things
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "regular speed");
	stepDistance *= 2;
}

void AEditorPawn::LeftTriggerStart()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("zoom in"));
	FollowCamera->FieldOfView = 35;
}

void AEditorPawn::LeftTriggerStop()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("zoom out"));
	FollowCamera->FieldOfView = 55;
}

void AEditorPawn::RightBumperStart()
{
	if (menuOpen)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("cycle right"));
		if (Buttons.IsValidIndex(CurrentButton + 1))
		{
			CurrentButton += 1;
		}
		else
		{
			CurrentButton = 0;
		}

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Current Button is: " + Buttons[CurrentButton]));
	}
}

void AEditorPawn::LeftBumper()
{
	if (menuOpen)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("cycle left"));
		if (Buttons.IsValidIndex(CurrentButton - 1))
		{
			CurrentButton -= 1;
		}
		else
		{
			CurrentButton = Buttons.Num() - 1;
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Current Button is: " + Buttons[CurrentButton]));
	}
}

void AEditorPawn::TopButton()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("menu"));
	if (!isBuildingSelected)
		Menu();
}

void AEditorPawn::RightButton()
{
	if (isBuildingSelected)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("revert selected item"));
		CancelMove();
	}
}

void AEditorPawn::LeftButton()
{
	if (isBuildingSelected)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("store item"));
		StoreBuilding();
	}


}

void AEditorPawn::BottomButton()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("pickup"));
	if (!menuOpen)
	{
		if (!isBuildingSelected)
		{
			//raycast to pick up building
			PickUpBuilding();
		}
		else
		{
			//if its not colliding with another building
			//place selected building at new spot
			PlaceBuilding();
		}
	}

}

void AEditorPawn::ResetMove()
{
	canMove = true;
	GetWorld()->GetTimerManager().ClearTimer(moveTimer);
}