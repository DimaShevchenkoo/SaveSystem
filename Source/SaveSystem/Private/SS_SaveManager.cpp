// Fill out your copyright notice in the Description page of Project Settings.

#include "SS_SaveManager.h"
#include "EngineUtils.h"
#include "GameplayTagAssetInterface.h"
#include "JsonUtils.h"
#include "Components/SS_IDComponent.h"
#include "Interfaces/SS_SavableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Saves/SS_SaveGame.h"

void USS_SaveManager::SaveGame(const FString& SlotName, bool bAsync)
{
	PendingSaveSlot = SlotName;
	bPendingSaveAsync = bAsync;


	USS_SaveGame* SaveGameInstance = Cast<USS_SaveGame>(UGameplayStatics::CreateSaveGameObject(USS_SaveGame::StaticClass()));
	SaveGameInstance->SlotName = SlotName;
	SaveGameInstance->SavedActors.Empty();
	SaveGameInstance->DestroyedPersistentActors = DestroyedPersistentActors;

	for (const TPair<FGuid, TWeakObjectPtr<AActor>>& Pair : SavableActors)
	{
		if (const AActor* Actor = Pair.Value.Get(); Actor && Actor->Implements<USS_SavableInterface>())
		{
			FSaveData Data;

			FString CustomData;
			TSharedPtr<FJsonObject> GeneratedCustomData = UJsonUtils::GenerateCustomData(Actor);
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&CustomData);
			FJsonSerializer::Serialize(GeneratedCustomData.ToSharedRef(), Writer);

			Data.CustomData = CustomData;
			Data.ActorID = Pair.Key;
			ISS_SavableInterface::Execute_Save(Actor, Data);

			UE_LOG(LogTemp, Warning, TEXT("%s ID: %s"), *Actor->GetName(), *Data.ActorID.ToString());
			UE_LOG(LogTemp, Warning, TEXT("%s"), *CustomData);
			SaveGameInstance->SavedActors.Add(Data);
		}
	}

	if (bAsync)
	{
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SlotName, 0,
		                                      FAsyncSaveGameToSlotDelegate::CreateUObject(this, &ThisClass::OnSaveFinished));
	}
	else
	{
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);
		UE_LOG(LogTemp, Log, TEXT("Saved game to slot %s (sync)"), *SlotName);
	}
}

void USS_SaveManager::OnSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Async Save to %s successful!"), *SlotName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Async Save to %s failed!"), *SlotName);
	}
}

void USS_SaveManager::LoadGame(const FString& SlotName, bool bAsync)
{
	if (bAsync)
	{
		UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0,
		                                        FAsyncLoadGameFromSlotDelegate::CreateUObject(this, &ThisClass::OnLoadFinished));
	}
	else
	{
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, 0);
		ApplyLoadedData(Cast<USS_SaveGame>(LoadedGame));
	}
}

void USS_SaveManager::AddDestroyedActor(const FGuid ID)
{
	if (SavableActors.Contains(ID))
	{
		SavableActors.Remove(ID);
	}
	DestroyedPersistentActors.Add(ID);
}

void USS_SaveManager::AddSavableActor(AActor* Actor)
{
	if (IsValid(Actor) && Actor->Implements<USS_SavableInterface>())
	{
		FGuid ID = ISS_SavableInterface::Execute_GetID(Actor);
		SavableActors.Add(ID, Actor);
		// Just in case
		DestroyedPersistentActors.Remove(ID);
	}
}

void USS_SaveManager::SetListOfSavableActors()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (It->IsValidLowLevel() && It->Implements<USS_SavableInterface>())
		{
			SavableActors.Add(ISS_SavableInterface::Execute_GetID(*It), *It);
		}
	}
}

void USS_SaveManager::OnLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGame)
{
	USS_SaveGame* Loaded = Cast<USS_SaveGame>(LoadedGame);
	if (!Loaded) return;

	ApplyLoadedData(Loaded);
}

void USS_SaveManager::ApplyLoadedData(USS_SaveGame* Loaded)
{
	DestroyedPersistentActors = Loaded->DestroyedPersistentActors;

	for (const FSaveData& Data : Loaded->SavedActors)
	{
		FGuid ID = Data.ActorID;

		if (!SavableActors.Contains(ID))
		{
			UE_LOG(LogTemp, Log, TEXT("No object found for ID: %s, Name: "), *Loaded->SlotName);
			continue;
		}
		const TWeakObjectPtr<AActor>* Actor = SavableActors.Find(ID);
		
		if (!Actor->IsValid() || !Actor->Get()->Implements<USS_SavableInterface>())
		{
			continue;
		}
		AActor* TargetActor = Actor->Get();

		if (DestroyedPersistentActors.Contains(ID))
		{
			TargetActor->Destroy();
			continue;
		}

		// Spawn
		if (!TargetActor)
		{
			UClass* Class = LoadObject<UClass>(nullptr, *Data.ActorClass);
			if (!Class) continue;

			UObject* CDO = Class->GetDefaultObject();
			if (HasGameplayTag(CDO, TAG_Save_ToSpawn))
			{
				TargetActor = GetWorld()->SpawnActor<AActor>(Class, Data.Location, Data.Rotation);

				if (TargetActor)
				{
					if (USS_IDComponent* IDComp = TargetActor->FindComponentByClass<USS_IDComponent>())
					{
						IDComp->ID = ID;
					}
					else
					{
						IDComp = NewObject<USS_IDComponent>(TargetActor);
						IDComp->RegisterComponent();
						TargetActor->AddInstanceComponent(IDComp);
						IDComp->ID = ID;
					}
				}
			}
			else
			{
				// if tag doesn't exist, it means that it's a static actor - so we don't spawn it
				// just skip the spawn
			}
		}

		// Load data
		UJsonUtils::LoadCustomData(TargetActor, Data);
		ISS_SavableInterface::Execute_Load(TargetActor, Data);
	}

	UE_LOG(LogTemp, Log, TEXT("Loaded data from slot %s"), *Loaded->SlotName);
}

bool USS_SaveManager::HasGameplayTag(UObject* Object, const FGameplayTag TagToCheck)
{
	if (!Object)
		return false;

	if (Object->Implements<UGameplayTagAssetInterface>())
	{
		if (const IGameplayTagAssetInterface* GameplayTagInterface = Cast<IGameplayTagAssetInterface>(Object))
		{
			FGameplayTagContainer Tags;
			GameplayTagInterface->GetOwnedGameplayTags(Tags);
			return Tags.HasTagExact(TagToCheck);
		}
	}
	return false;
}
