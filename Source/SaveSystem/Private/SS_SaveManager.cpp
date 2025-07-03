// Fill out your copyright notice in the Description page of Project Settings.

#include "SS_SaveManager.h"
#include "EngineUtils.h"
#include "GameplayTagAssetInterface.h"
#include "JsonUtils.h"
#include "Components/SS_AutoIDComponent.h"
#include "Interfaces/SS_SavableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Saves/SS_SaveGame.h"

void USS_SaveManager::SaveGame(UWorld* World, const FString& SlotName, bool bAsync) {
	CachedWorld = World;
	PendingSaveSlot = SlotName;
	bPendingSaveAsync = bAsync;


	USS_SaveGame* SaveGameInstance = Cast<USS_SaveGame>(UGameplayStatics::CreateSaveGameObject(USS_SaveGame::StaticClass()));
	SaveGameInstance->SlotName = SlotName;
	SaveGameInstance->SavedActors.Empty();

	for (TActorIterator<AActor> It(World); It; ++It) {
		if (const AActor* Actor = *It; Actor && Actor->GetClass()->ImplementsInterface(USS_SavableInterface::StaticClass())) {
			FSaveData Data;

			FString CustomData;
			TSharedPtr<FJsonObject> GeneratedCustomData = UJsonUtils::GenerateCustomData(Actor);
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&CustomData);
			FJsonSerializer::Serialize(GeneratedCustomData.ToSharedRef(), Writer);

			// Now JsonString contains your JSON as text
			UE_LOG(LogTemp, Warning, TEXT("%s"), *CustomData);
			Data.CustomData = CustomData;
			ISS_SavableInterface::Execute_Save(Actor, Data);
			SaveGameInstance->SavedActors.Add(Data);
		}
	}

	if (bAsync) {
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SlotName, 0,
		                                      FAsyncSaveGameToSlotDelegate::CreateUObject(this, &ThisClass::OnSaveFinished));
	} else {
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);
		UE_LOG(LogTemp, Log, TEXT("Saved game to slot %s (sync)"), *SlotName);
	}
}

void USS_SaveManager::OnSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess) {
	if (bSuccess) {
		UE_LOG(LogTemp, Log, TEXT("Async Save to %s successful!"), *SlotName);
	} else {
		UE_LOG(LogTemp, Error, TEXT("Async Save to %s failed!"), *SlotName);
	}
}

void USS_SaveManager::LoadGame(UWorld* World, const FString& SlotName, bool bAsync) {
	if (bAsync) {
		UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0,
		                                        FAsyncLoadGameFromSlotDelegate::CreateUObject(this, &ThisClass::OnLoadFinished));
	} else {
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, 0);
		ApplyLoadedData(World, Cast<USS_SaveGame>(LoadedGame));
	}
}

void USS_SaveManager::OnLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGame) {
	USS_SaveGame* Loaded = Cast<USS_SaveGame>(LoadedGame);
	if (!Loaded) return;

	UWorld* World = GetWorld();
	ApplyLoadedData(World, Loaded);
}

void USS_SaveManager::ApplyLoadedData(UWorld* World, USS_SaveGame* Loaded) {
	for (const FSaveData& Data : Loaded->SavedActors) {
		AActor* TargetActor = nullptr;

		// Try to find actor with its ID
		for (TActorIterator<AActor> It(World); It; ++It) {
			AActor* Candidate = *It;
			if (const USS_AutoIDComponent* IDComp = Candidate->FindComponentByClass<USS_AutoIDComponent>()) {
				if (IDComp->ActorID == Data.ActorID) {
					TargetActor = Candidate;
					break;
				}
			}
		}

		// Spawn
		if (!TargetActor) {
			UClass* Class = LoadObject<UClass>(nullptr, *Data.ActorClass);
			if (!Class) continue;

			UObject* CDO = Class->GetDefaultObject();
			if (HasGameplayTag(CDO, TAG_Save_ToSpawn)) {
				TargetActor = World->SpawnActor<AActor>(Class, Data.Location, Data.Rotation);

				if (TargetActor) {
					if (USS_AutoIDComponent* IDComp = TargetActor->FindComponentByClass<USS_AutoIDComponent>()) {
						IDComp->ActorID = Data.ActorID;
					} else {
						IDComp = NewObject<USS_AutoIDComponent>(TargetActor);
						IDComp->RegisterComponent();
						TargetActor->AddInstanceComponent(IDComp);
						IDComp->ActorID = Data.ActorID;
					}
				}
			} else {
				// if tag doesn't exist, it means that it's a static actor - so we don't spawn it
				// just skip the spawn
			}
		}

		// Load data
		if (TargetActor && TargetActor->GetClass()->ImplementsInterface(USS_SavableInterface::StaticClass())) {
			// UJsonUtils::LoadCustomData(TargetActor, Data);
			ISS_SavableInterface::Execute_Load(TargetActor, Data);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Loaded data from slot %s"), *Loaded->SlotName);
}

bool USS_SaveManager::HasGameplayTag(UObject* Object, const FGameplayTag TagToCheck) {
	if (!Object)
		return false;

	if (Object->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass())) {
		if (const IGameplayTagAssetInterface* GameplayTagInterface = Cast<IGameplayTagAssetInterface>(Object)) {
			FGameplayTagContainer Tags;
			GameplayTagInterface->GetOwnedGameplayTags(Tags);
			return Tags.HasTagExact(TagToCheck);
		}
	}
	return false;
}
