// Fill out your copyright notice in the Description page of Project Settings.


#include "GrowGun.h"
#include "GunPuzzleDemo.h"


AGrowGun::AGrowGun()
{

}

void AGrowGun::ProjectileFire()
{
  Super::ProjectileFire();

  if (ProjectileClass != NULL)
  {
    FVector GunOffset = FVector(100.0f, 0.0f, 10.0f);
    UWorld* const World = GetWorld();
    if (World != NULL)
    {
      FRotator MFRot = WeaponMesh->GetSocketRotation("MuzzleFlash");
      FVector MFLoc = WeaponMesh->GetSocketLocation("MuzzleFlash") + MFRot.RotateVector(GunOffset);
      FActorSpawnParameters SpawnParams;
      SpawnParams.Owner = this;
      SpawnParams.Instigator = Instigator;
      SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
      World->SpawnActor<AGrowBullet>(ProjectileClass, MFLoc, MFRot, SpawnParams);
    }
  }
}
