// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ShrinkBullet.h"
#include "ShrinkGun.generated.h"

/**
 * 
 */
UCLASS()
class GUNPUZZLEDEMO_API AShrinkGun : public AWeapon
{
	GENERATED_BODY()

    virtual void ProjectileFire() override;

  UPROPERTY(EditDefaultsOnly, Category = Projectile)
    TSubclassOf<class AShrinkBullet> ProjectileClass;

public:
  AShrinkGun();
	
};
