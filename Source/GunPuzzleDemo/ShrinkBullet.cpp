// Fill out your copyright notice in the Description page of Project Settings.


#include "ShrinkBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AShrinkBullet::AShrinkBullet()
{
  // Use a sphere as a simple collision representation
  CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
  CollisionComp->InitSphereRadius(5.0f);
  CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
  CollisionComp->OnComponentHit.AddDynamic(this, &AShrinkBullet::OnHit);		// set up a notification for when this component hits something blocking

  // Players can't walk on it
  CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
  CollisionComp->CanCharacterStepUpOn = ECB_No;

  // Set as root component
  RootComponent = CollisionComp;

  // Use a ProjectileMovementComponent to govern this projectile's movement
  ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
  ProjectileMovement->UpdatedComponent = CollisionComp;
  ProjectileMovement->InitialSpeed = 3000.f;
  ProjectileMovement->MaxSpeed = 3000.f;
  ProjectileMovement->bRotationFollowsVelocity = false;
  ProjectileMovement->bShouldBounce = false;

  // Die after 3 seconds by default
  InitialLifeSpan = 3.0f;
}

void AShrinkBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
  // Only add impulse and destroy projectile if we hit a physics

  if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
  {
    FVector Scale = OtherComp->GetComponentScale(); //Get the size of the other comp
    Scale *= 0.8f; //Make it slightly smaller

    if (Scale.GetMin() < 0.5f) //If the object gets very small
    {
      OtherActor->Destroy(); //Destory it
    }
    else
    {
      OtherComp->SetWorldScale3D(Scale); //Update the other comp with its new smaller size
    }
  }

    Destroy(); //Destory bullet
  

}

