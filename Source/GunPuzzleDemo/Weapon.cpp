// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Engine.h"


// Sets default values
AWeapon::AWeapon()
{

  CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
  RootComponent = CollisionComp;

  WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->bCastDynamicShadow = false;
	WeaponMesh->CastShadow = false;
  WeaponMesh->SetupAttachment(RootComponent);

}

void AWeapon::Fire()
{
  if (ProjectileType == EWeaponProjectile::EShrink)
  {
    ProjectileFire();
  }
  if (ProjectileType == EWeaponProjectile::EPush)
  {
      ProjectileFire();
  }
  if (ProjectileType == EWeaponProjectile::EGrow)
  {
    ProjectileFire();
  }

}

void AWeapon::ProjectileFire()
{
 
}




