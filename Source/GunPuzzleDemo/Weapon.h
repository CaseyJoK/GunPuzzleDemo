// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

#define Trace_Weapon ECC_GameTraceChannel1

UENUM(BlueprintType)
namespace EWeaponProjectile
{
  enum ProjectileType
  {
    EShrink UMETA(DisplayName = "ShrinkBullet"),
    EPush UMETA(DisplayName = "PushBullet"),
    EGrow UMETA(DisplayName = "GrowBullet"),
  };
}

USTRUCT()
struct FWeaponData
{
  GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Config)
    float TimeBetweenShots;

  UPROPERTY(EditAnywhere, Category = Config)
    float WeaponSpread;

  UPROPERTY(EditAnywhere, Category = Config)
    float WeaponRange;

  UPROPERTY(EditAnywhere, Category = Config)
    FString Name;

};

UCLASS()
class GUNPUZZLEDEMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

  UFUNCTION()
  void Fire();
   

  UFUNCTION()
  virtual void ProjectileFire();

  UPROPERTY(EditDefaultsOnly, Category = Config)
  FWeaponData WeaponConfig;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Config)
  TEnumAsByte<EWeaponProjectile::ProjectileType> ProjectileType;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Collision)
  UBoxComponent* CollisionComp;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Config)
  USkeletalMeshComponent* WeaponMesh;


protected:


};
