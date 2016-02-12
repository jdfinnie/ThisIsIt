// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ThisIsIt.h"
#include "ThisIsItGameMode.h"
#include "ThisIsItCharacter.h"
#include "BaseCharacter.h"
#include "Blueprint/UserWidget.h"

AThisIsItGameMode::AThisIsItGameMode()
{
	// Set player pawn
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//set enemy(s)
	static ConstructorHelpers::FClassFinder<ABaseCharacter> EnemyPawnBPClass(TEXT("Blueprint'/Game/Characters/Attacker'"));
	if (EnemyPawnBPClass.Class != NULL)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "I FOUND THE CHARACTER!!");
		test = EnemyPawnBPClass.Class;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "CHARACTER WAS NULL");
	}


}

void AThisIsItGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != NULL) //if we have attached a hud via blueprint
	{
		currentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (currentWidget != NULL)
		{
			currentWidget->AddToViewport();
		}
	}

	//spawn initial enemies and allies

	//add enemies to list for tracking

	//start round timer

}

void AThisIsItGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//check all enemies to see if dead

	//if all enemies dead advance to next round

	//or if timer runs down spawn next wave anyways

	//if building health = 0 game over

	//if player lives  = 0 game over

	//if all rounds completed, and player/hub alive WIN

}

void AThisIsItGameMode::SpawnCharacter(TSubclassOf<class ABaseCharacter> actor, FVector location, FRotator rotation)
{
	UWorld* const World = GetWorld();
	if (World)
	{
		if (actor != NULL)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Instigator = NULL;

			ABaseCharacter* temp = World->SpawnActor<ABaseCharacter>(actor, location, rotation, spawnParams);
		}
	}
}
