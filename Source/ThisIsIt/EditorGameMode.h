// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "EditorGameMode.generated.h"

UCLASS(minimalapi)
class AEditorGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AEditorGameMode();
	virtual void Tick(float DeltaSeconds) override; //may be needed later
	virtual void BeginPlay() override;

	//UPROPERTY()
		//TSubclassOf<class ABaseCharacter> test;

	//void SpawnCharacter(TSubclassOf<class ABaseCharacter> actor, FVector location, FRotator rotation);

protected:

	//https://www.youtube.com/watch?v=jbM4VU8oPYI
	//widget class to use for hud
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD?")
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	//the instance of the hud
	UPROPERTY()
	class UUserWidget* currentWidget;
};

