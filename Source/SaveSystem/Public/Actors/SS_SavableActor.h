// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SS_SavableInterface.h"
#include "SS_SavableActor.generated.h"

class USS_IDComponent;

UCLASS()
class SAVESYSTEM_API ASS_SavableActor : public AActor, public ISS_SavableInterface
{
	GENERATED_BODY()

public:
	ASS_SavableActor();

	/* Begin ISS_SavableInterface implementation */
	virtual void Save_Implementation(FSaveData& OutData) const override;
	virtual void Load_Implementation(const FSaveData& InData) override;
	virtual void Destroyed() override;
	virtual FGuid GetID_Implementation() override;

	/* End ISS_SavableInterface implementation */
	
protected:
	// Save System
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USS_IDComponent> IDComponent;
};
