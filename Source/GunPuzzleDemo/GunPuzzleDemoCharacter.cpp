// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GunPuzzleDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Engine.h"
#include "Engine/Blueprint.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGunPuzzleDemoCharacter

AGunPuzzleDemoCharacter::AGunPuzzleDemoCharacter()
{
  CurrentWeapon = NULL;

  Inventory.SetNum(3, false); //Creates the size of our inventory

 
  // Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

  //Collision Comp
  CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Comp"));
  CollisionComp->SetupAttachment(RootComponent);
  CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AGunPuzzleDemoCharacter::OnCollision); //When something enters the collision call the function

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	//Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

}

void AGunPuzzleDemoCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGunPuzzleDemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

  //Equip Each Gun
  PlayerInputComponent->BindAction("EquipShrinkGun", IE_Pressed, this, &AGunPuzzleDemoCharacter::EquipShrinkGun);
  PlayerInputComponent->BindAction("EquipGrowGun", IE_Pressed, this, &AGunPuzzleDemoCharacter::EquipGrowGun);
  PlayerInputComponent->BindAction("EquipPushGun", IE_Pressed, this, &AGunPuzzleDemoCharacter::EquipPushGun);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGunPuzzleDemoCharacter::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGunPuzzleDemoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGunPuzzleDemoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGunPuzzleDemoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGunPuzzleDemoCharacter::LookUpAtRate);
}

void AGunPuzzleDemoCharacter::OnCollision(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  AShrinkGun *ShrinkGun = Cast<AShrinkGun>(OtherActor); 

  if (ShrinkGun) //Checks if the OtherActor is the shrink gun
  {
    Inventory[0] = ShrinkGun->GetClass(); //If it is, store the class in first inventory slot
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "I just picked up a " + ShrinkGun->WeaponConfig.Name);
    ShrinkGun->Destroy();

  }

  AGrowGun *GrowGun = Cast<AGrowGun>(OtherActor);

  if (GrowGun) //Checks if the OtherActor is the grow gun
  {
    Inventory[1] = GrowGun->GetClass(); //If it is, store the class in second inventory slot
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "I just picked up a " + GrowGun->WeaponConfig.Name);
    GrowGun->Destroy();

  }

  APushGun *PushGun = Cast<APushGun>(OtherActor);

  if (PushGun) //Checks if the OtherActor is the push gun
  {
    Inventory[2] = PushGun->GetClass(); //If it is, store the class in third inventory slot
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "I just picked up a " + PushGun->WeaponConfig.Name);
    PushGun->Destroy();

  }

}

void AGunPuzzleDemoCharacter::EquipShrinkGun()
{
  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.Instigator = Instigator;

  AWeapon *Spawner = GetWorld()->SpawnActor<AWeapon>(Inventory[0], SpawnParams);

  if (Spawner)
  {
    if (CurrentWeapon != NULL)
    {

      for (int32 i = 0; i < 3; i++)
      {
        if (Inventory[i] != NULL && Inventory[i]->GetDefaultObject<AWeapon>()->WeaponConfig.Name == CurrentWeapon->WeaponConfig.Name) //Check each slot in the inventory in order to put back the current weapon in its correct slot
        {
          Inventory[i] = NULL;
          Inventory[i] = CurrentWeapon->GetClass();
          GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "I put " + CurrentWeapon->WeaponConfig.Name + " away in slot " + FString::FromInt(i));
        }
      }

      CurrentWeapon->Destroy();
      Spawner->CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); //Turns off the collision on the weapon when equipping
      Spawner->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attached Spawned Gun to Player Mesh
      CurrentWeapon = Spawner;
      GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "Current Weapon is " + CurrentWeapon->WeaponConfig.Name);
    }
    else
    {
      Spawner->CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); //Turns off the collision on the weapon when equipping
      Spawner->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attached Spawned Gun to Player Mesh
      CurrentWeapon = Spawner;
      GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "Current Weapon is " + CurrentWeapon->WeaponConfig.Name);
    }
  }
}
void AGunPuzzleDemoCharacter::EquipGrowGun()
{
  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.Instigator = Instigator;

  AWeapon *Spawner = GetWorld()->SpawnActor<AWeapon>(Inventory[1], SpawnParams);

  if (Spawner)
  {
    if (CurrentWeapon != NULL)
    {

      for (int32 i = 0; i < 3; i++)
      {
        if (Inventory[i] != NULL && Inventory[i]->GetDefaultObject<AWeapon>()->WeaponConfig.Name == CurrentWeapon->WeaponConfig.Name) //Check each slot in the inventory in order to put back the current weapon in its correct slot
        {
          Inventory[i] = NULL;
          Inventory[i] = CurrentWeapon->GetClass();
          GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "I put " + CurrentWeapon->WeaponConfig.Name + " away in slot " + FString::FromInt(i));
        }
      }

      CurrentWeapon->Destroy();
      Spawner->CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); //Turns off the collision on the weapon when equipping
      Spawner->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attached Spawned Gun to Player Mesh
      CurrentWeapon = Spawner;
      GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "Current Weapon is " + CurrentWeapon->WeaponConfig.Name);
    }
    else
    {
      Spawner->CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); //Turns off the collision on the weapon when equipping
      Spawner->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attached Spawned Gun to Player Mesh
      CurrentWeapon = Spawner;
      GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "Current Weapon is " + CurrentWeapon->WeaponConfig.Name);
    }
  }
  
}
void AGunPuzzleDemoCharacter::EquipPushGun()
{
  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.Instigator = Instigator;

  AWeapon *Spawner = GetWorld()->SpawnActor<AWeapon>(Inventory[2], SpawnParams);

  if (Spawner)
  {
    if (CurrentWeapon != NULL)
    {

      for (int32 i = 0; i < 3; i++)
      {
        if (Inventory[i] != NULL && Inventory[i]->GetDefaultObject<AWeapon>()->WeaponConfig.Name == CurrentWeapon->WeaponConfig.Name) //Check each slot in the inventory in order to put back the current weapon in its correct slot
        {
          Inventory[i] = NULL;
          Inventory[i] = CurrentWeapon->GetClass();
          GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "I put " + CurrentWeapon->WeaponConfig.Name + " away in slot " + FString::FromInt(i));
        }
      }

      CurrentWeapon->Destroy();
      Spawner->CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); //Turns off the collision on the weapon when equipping
      Spawner->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attached Spawned Gun to Player Mesh
      CurrentWeapon = Spawner;
      GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "Current Weapon is " + CurrentWeapon->WeaponConfig.Name);
    }
    else
    {
      Spawner->CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); //Turns off the collision on the weapon when equipping
      Spawner->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attached Spawned Gun to Player Mesh
      CurrentWeapon = Spawner;
      GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "Current Weapon is " + CurrentWeapon->WeaponConfig.Name);
    }
  }
}

void AGunPuzzleDemoCharacter::OnFire()
{
  if (CurrentWeapon != NULL)
  {
    CurrentWeapon->Fire();
  }
  else
  {
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "No Weapon currently equipped");
  }
}


void AGunPuzzleDemoCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGunPuzzleDemoCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGunPuzzleDemoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGunPuzzleDemoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

