// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SS_IDComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SAVESYSTEM_API USS_IDComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
#if WITH_EDITOR

	virtual void PostLoad() override;
	virtual void PostEditImport() override;
	virtual void OnRegister() override;
#endif
	
	UPROPERTY(VisibleAnywhere, SaveGame)
	FGuid ID;
};
