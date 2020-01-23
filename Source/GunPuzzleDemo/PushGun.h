// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "PushBullet.h"
#include "PushGun.generated.h"

UCLASS()
class GUNPUZZLEDEMO_API APushGun : public AWeapon
{
  GENERATED_BODY()

    virtual void ProjectileFire() override;

  UPROPERTY(EditDefaultsOnly, Category = Projectile)
    TSubclassOf<class APushBullet> ProjectileClass;
	
public:
  APushGun();
};
