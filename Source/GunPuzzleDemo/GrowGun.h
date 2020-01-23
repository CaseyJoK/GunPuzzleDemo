// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GrowBullet.h"
#include "GrowGun.generated.h"

/**
 * 
 */
UCLASS()
class GUNPUZZLEDEMO_API AGrowGun : public AWeapon
{
	GENERATED_BODY()

   virtual void ProjectileFire() override;

  UPROPERTY(EditDefaultsOnly, Category = Projectile)
    TSubclassOf<class AGrowBullet> ProjectileClass;

public:
  AGrowGun();
};

