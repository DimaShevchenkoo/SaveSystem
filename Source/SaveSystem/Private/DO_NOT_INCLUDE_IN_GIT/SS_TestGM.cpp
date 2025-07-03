// Fill out your copyright notice in the Description page of Project Settings.

#include "DO_NOT_INCLUDE_IN_GIT/SS_TestGM.h"

#include "SS_SaveManager.h"

void ASS_TestGM::SaveGame()
{
	SaveManager->SaveGame(GetWorld(), "Abiba");
}

void ASS_TestGM::LoadGame()
{
	SaveManager->LoadGame(GetWorld(), "Abiba");
}

void ASS_TestGM::BeginPlay()
{
	Super::BeginPlay();

	SaveManager = NewObject<USS_SaveManager>();
}
