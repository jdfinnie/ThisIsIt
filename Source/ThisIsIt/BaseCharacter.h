// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum  class Type
{
	Human		UMETA(DisplayName = "Human"),
	Robot		UMETA(DisplayName = "Robot"),
};

UENUM(BlueprintType)
enum  class State
{
	Idle			UMETA(DisplayName = "Idle"),
	Seeking			UMETA(DisplayName = "Seeking"),
	Attacking		UMETA(DisplayName = "Attacking"),
	Dead			UMETA(DisplayName = "Dead"),
};

USTRUCT()
struct FBaseStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
		int32 health_Max;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
		int32 energy_Max;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
		int32 stamina_Max;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
		int32 moveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStats)
		int32 armorValue;

	UPROPERTY(EditDefaultsOnly, Category = Weapons)
		TSubclassOf<class AWeapon> primaryWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Weapons)
		TSubclassOf<class AWeapon> secondaryWeapon;
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "potential targets")
	TArray<ACharacter*> potentialTargets;

	//calculate death helper function
	virtual void CalculateDead();

	//calculate health function
	UFUNCTION(BlueprintCallable, Category = "Base Character")
		virtual void CalculateHealth(float delta);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Config)
		TEnumAsByte<Type> type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Config)
		TEnumAsByte<State> state;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FBaseStats stats;

	//UPROPERTY(EditDefaultsOnly, Category = Inventory)
	//	TSubclassOf<class AWeapon> Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	bool isAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	bool isRunning;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	bool isJumping;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	bool isAiming;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	bool tryJump;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	bool isReloading;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	bool isSwapingWeapons;

	UFUNCTION(BlueprintCallable, Category = "Base Character")
	void ReloadComplete();
	//void SwapWeaponsComplete();
	
	bool CanShoot(); // ? !reloading, switching,jumping,running

	
	Type GetType();

	State defaultState;
};
