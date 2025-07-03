// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "SS_SavableInterface.generated.h"

USTRUCT(BlueprintType)
struct FSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FString ActorClass;

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY()
	FString ActorID;

	UPROPERTY()
	TArray<FString> CustomData;
};

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Save_ToSpawn);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Save_Static);

UINTERFACE()
class USS_SavableInterface : public UInterface
{
	GENERATED_BODY()
};

class SAVESYSTEM_API ISS_SavableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save")
	void Save(FSaveData& OutData) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save")
	void Load(const FSaveData& InData);

};
