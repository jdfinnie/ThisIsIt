// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseCharacter.h"
#include "Weapon.generated.h"

class AThisIsItCharacter;

#define TRACE_WEAPON ECC_GameTraceChannel1

UENUM(BlueprintType)
enum  class WeaponType
{
	OneHand		UMETA(DisplayName = "1-Hand"),
	TwoHand		UMETA(DisplayName = "2-Hand"),
	Heavy		UMETA(DisplayName = "Heavy"),
	Melee		UMETA(DisplayName = "Melee"),
};

UENUM(BlueprintType)
enum  class ProjectileType
{
	Bullet		UMETA(DisplayName = "Bullet"),
	Spread		UMETA(DisplayName = "Spread"),
	Projectile	UMETA(DisplayName = "Projectile"),
};

UENUM(BlueprintType)
enum  class FireMode
{
	Single		UMETA(DisplayName = "Single"),
	Burst		UMETA(DisplayName = "Burst"),
	Auto		UMETA(DisplayName = "Automatic"),
};

USTRUCT()
public struct FWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
		int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
		int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		int32 MaxClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		float TimeBetweenShots;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	//	int32 ShotCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		float WeaponRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		float WeaponSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FString Name;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	//	UTexture2D* SplashArt;

};

/**
*
*/
UCLASS()
class THISISIT_API AWeapon : public AActor
{
	GENERATED_BODY()

		//UPROPERTY(EditAnywhere, Category = Mesh)
	//class USkeletalMeshComponent* gunMesh;

public:
	// Sets default values for this character's properties
	AWeapon();

	UFUNCTION(BlueprintCallable, Category = Event)
		void Fire();

	UFUNCTION()
		void Instant_Fire();

	UFUNCTION()
		virtual void ProjectileFire(); // this will eventually be inherited from a projectile subclass

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FWeaponInfo WeaponInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Config)
		TEnumAsByte<ProjectileType> ProjectileType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Config)
		TEnumAsByte<FireMode> FireMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Config)
		TEnumAsByte<WeaponType> WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
		UBoxComponent *CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Config)
		USkeletalMeshComponent* WeaponMesh;

	//UPROPERTY(EditDefaultsOnly, Category = Projectile)
	//	TSubclassOf<class ARocket> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Config)
		USoundCue *FireSound;

	UPROPERTY(EditAnywhere)
		UParticleSystem* FireEffect;

	UPROPERTY(EditAnywhere)
		UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere)
		UAnimSequence* FireAnim;

	UPROPERTY(EditAnywhere)
		UAnimSequence* ReloadAnim;

	//not sure about these last two...
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Config)
		int32 CurrentAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Config)
		int32 CurrentClip;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	void SetOwningPawn(ACharacter *NewOwner);

	void AttachToPlayer();
	void DetachFromPlayer();

	void OnEquip();
	void OnUnEquip();

	//can or should be shooting?
	bool canFire;
	bool isFiring;

	FTimerHandle fireTimer;

	FTimerHandle reloadTimer;

	//reloading
	//bool isReloading;
	//bool reloadComplete;

	void FireBegin();

	void FireEnd();

	void Reload();
	void ReloadComplete();


	UAudioComponent* PlayWeaponSound(USoundCue *Sound);


protected:

	FHitResult WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo) const;

	void ProcessInstantHit(const FHitResult &Impact, const FVector &Origin, const FVector &ShootDir, int32 RandomSeed, float ReticleSpread);

	ACharacter *MyPawn;

};
