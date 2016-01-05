// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum  class NPCType
{
	Human		UMETA(DisplayName = "Human"),
	Robot		UMETA(DisplayName = "Robot"),
};

UENUM(BlueprintType)
enum  class NPCState
{
	Idle			UMETA(DisplayName = "Idle"),
	Seeking			UMETA(DisplayName = "Seeking"),
	Attacking		UMETA(DisplayName = "Attacking"),
	Dead			UMETA(DisplayName = "Dead"),
};

UCLASS(Blueprintable)
class THISISIT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	//health property
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base Character")
		float health = 100;

	//alive?
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Base Character")
		bool isDead = false;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base Character")
		ACharacter *target;

	//calculate death helper function
	virtual void CalculateDead();

	//calculate health function
	UFUNCTION(BlueprintCallable, Category = "Base Character")
		virtual void CalculateHealth(float delta);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Config)
		TEnumAsByte<NPCType> npcType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Config)
		TEnumAsByte<NPCState> npcState;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TSubclassOf<class AWeapon> Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
		AWeapon *CurrentWeapon;

#if WITH_EDITOR
	//editor-centric code for changing properties
	virtual void PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent)
		override;
#endif

	// Sets default values for this character's properties
	ABaseCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void GiveDefaultWeapon();
	void EquipWeapon(AWeapon *Weapon);

	void Attack();

	bool isAttacking;
	
	NPCType GetNPCType();

	NPCState defaultState;
};
