// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "ThisIsItGameMode.generated.h"

UCLASS(minimalapi)
class AThisIsItGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AThisIsItGameMode();
	virtual void Tick(float DeltaSeconds) override; //may be needed later
	virtual void BeginPlay() override;

	UPROPERTY()
	TSubclassOf<class ABaseCharacter> test;

	void SpawnCharacter(TSubclassOf<class ABaseCharacter> actor, FVector location, FRotator rotation);

protected:

	//https://www.youtube.com/watch?v=jbM4VU8oPYI
	//widget class to use for hud
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD?")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	//the instance of the hud
	UPROPERTY()
	class UUserWidget* currentWidget;
};



