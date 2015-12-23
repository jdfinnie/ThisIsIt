// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Weapon.h"
#include "ThisIsItCharacter.generated.h"

UCLASS(config=Game)
class AThisIsItCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:

	int invert;

	AThisIsItCharacter();

	/*COLLISION FOR CHARACTER*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
		UBoxComponent* CollisionComp;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TSubclassOf<class AWeapon> PrimaryWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TSubclassOf<class AWeapon> SecondaryWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
		AWeapon *CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
		bool isRunning;

protected:

	//AWeapon *gun;
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void RightTriggerStart();
	void RightTriggerStop();

	void LeftTriggerStart();
	void LeftTriggerStop();

	void RightBumperStart();
	void RightBumperStop();

	void LeftBumper();

	void TopButton();
	void RightButton();
	void LeftButton();

	void GiveDefaultWeapon();
	void EquipWeapon(AWeapon *Weapon);
	void SwitchWeapons();


	UFUNCTION(BlueprintCallable, Category = Event)
		virtual void BeginPlay() override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

