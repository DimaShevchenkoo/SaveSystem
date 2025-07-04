// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "Saves/SS_SaveGame.h"
#include "UObject/Object.h"
#include "SS_SaveManager.generated.h"

class ISS_SavableInterface;
DECLARE_MULTICAST_DELEGATE(FOnGameSavedDelegate);

UCLASS()
class SAVESYSTEM_API USS_SaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SaveGame(const FString& SlotName, bool bAsync = false);
	
	UFUNCTION(BlueprintCallable)
	void LoadGame(const FString& SlotName, bool bAsync = false);
	
	UFUNCTION(BlueprintCallable)
	void AddDestroyedActor(FGuid ID);
	
	UFUNCTION(BlueprintCallable)
	void AddSavableActor(AActor* Actor);

	// Should be called after all initial actors are spawned
	UFUNCTION(BlueprintCallable)
	void SetListOfSavableActors();

private:
	void OnSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	void OnLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGame);
	void ApplyLoadedData(USS_SaveGame* Loaded);
	static bool HasGameplayTag(UObject* Object, FGameplayTag TagToCheck);

	FString PendingSaveSlot;
	bool bPendingSaveAsync;

	UPROPERTY(VisibleAnywhere)
	TSet<FGuid> DestroyedPersistentActors;
	UPROPERTY(VisibleAnywhere)
	TMap<FGuid, TWeakObjectPtr<AActor>> SavableActors;

};
