// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SS_SavableInterface.h"
#include "GameFramework/Character.h"
#include "SS_SavableCharacter.generated.h"

class USS_AutoIDComponent;

UCLASS()
class SAVESYSTEM_API ASS_SavableCharacter : public ACharacter, public ISS_SavableInterface
{
	GENERATED_BODY()

public:
	ASS_SavableCharacter();

	/* Begin ISS_SavableInterface implementation */
	virtual void Save_Implementation(FSaveData& OutData) const override;
	virtual void Load_Implementation(const FSaveData& InData) override;
	virtual void AssignID() override;
	/* End ISS_SavableInterface implementation */
	
protected:
	// Save System
	UPROPERTY()
	TObjectPtr<USS_AutoIDComponent> AutoIDComponent;

	UPROPERTY(SaveGame)
	bool bAboba = false;

	UPROPERTY(SaveGame)
	FString Gad = "GADOST";

	UPROPERTY(SaveGame)
	FName GadName = "GadName";

	UPROPERTY(SaveGame)
	FText GadText = FText::FromString("GadText");

	UPROPERTY(SaveGame)
	TSet<int32> GadKeys = { 1, 21, 44, 11,12, 634 };
	
	UPROPERTY(SaveGame)
	TMap<FString, int32> Gadi = {{"Gad", 231}, {"Gad2", 22133}};
};
