// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "Saves/SS_SaveGame.h"
#include "UObject/Object.h"
#include "SS_SaveManager.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGameSavedDelegate);

UCLASS()
class SAVESYSTEM_API USS_SaveManager : public UObject
{
	GENERATED_BODY()

public:
	void SaveGame(UWorld* World, const FString& SlotName, bool bAsync = false);
	void LoadGame(UWorld* World, const FString& SlotName, bool bAsync = false);

private:
	void OnSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	void OnLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGame);
	void ApplyLoadedData(UWorld* World, USS_SaveGame* Loaded);
	static bool HasGameplayTag(UObject* Object, FGameplayTag TagToCheck);

	UPROPERTY()
	UWorld* CachedWorld;
	
	FString PendingSaveSlot;
	bool bPendingSaveAsync;
};
