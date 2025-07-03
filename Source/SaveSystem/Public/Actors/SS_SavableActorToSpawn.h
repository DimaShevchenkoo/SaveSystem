// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SS_SavableActor.h"
#include "SS_SavableActorToSpawn.generated.h"

UCLASS()
class SAVESYSTEM_API ASS_SavableActorToSpawn : public ASS_SavableActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	ASS_SavableActorToSpawn();

	/* Begin IGameplayTagAssetInterface implementation */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	/* End IGameplayTagAssetInterface implementation */
	
	/* Begin ISS_SavableInterface implementation */
	virtual void Save_Implementation(FSaveData& OutData) const override;
	virtual void Load_Implementation(const FSaveData& InData) override;
	/* End ISS_SavableInterface implementation */

protected:
	FGameplayTagContainer SavableTags;
};
