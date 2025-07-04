// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SS_SaveGame.generated.h"

struct FSaveData;

UCLASS()
class SAVESYSTEM_API USS_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	FString SlotName;

	UPROPERTY(SaveGame)
	TArray<FSaveData> SavedActors;

	UPROPERTY(SaveGame)
	TSet<FGuid> DestroyedPersistentActors; 
};
