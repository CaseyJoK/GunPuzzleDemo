// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GunPuzzleDemoGameMode.h"
#include "GunPuzzleDemoHUD.h"
#include "GunPuzzleDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGunPuzzleDemoGameMode::AGunPuzzleDemoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGunPuzzleDemoHUD::StaticClass();
}
