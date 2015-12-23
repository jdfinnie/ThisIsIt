// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ThisIsIt.h"
#include "ThisIsItGameMode.h"
#include "ThisIsItCharacter.h"
#include "Blueprint/UserWidget.h"

AThisIsItGameMode::AThisIsItGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//if (HUDWidgetClass != NULL) 
	//{
	//	currentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
	//	if (currentWidget != NULL)
	//	{
	//		currentWidget->AddToViewport();
	//	}

	//}
}

void AThisIsItGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//do other stuff as needed
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
}
