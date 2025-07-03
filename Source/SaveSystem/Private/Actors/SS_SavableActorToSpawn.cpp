// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/SS_SavableActorToSpawn.h"

ASS_SavableActorToSpawn::ASS_SavableActorToSpawn()
{
	PrimaryActorTick.bCanEverTick = false;

	SavableTags.AddTag(TAG_Save_ToSpawn);
}

void ASS_SavableActorToSpawn::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(SavableTags);
}

bool ASS_SavableActorToSpawn::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return SavableTags.HasTag(TagToCheck);
}

bool ASS_SavableActorToSpawn::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return SavableTags.HasAll(TagContainer);
}

bool ASS_SavableActorToSpawn::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return SavableTags.HasAny(TagContainer);
}

void ASS_SavableActorToSpawn::Save_Implementation(FSaveData& OutData) const
{
	Super::Save_Implementation(OutData);

	OutData.Location = GetActorLocation();
	OutData.Rotation = GetActorRotation();
	OutData.ActorClass = GetClass()->GetPathName();
}

void ASS_SavableActorToSpawn::Load_Implementation(const FSaveData& InData)
{
	Super::Load_Implementation(InData);

	SetActorLocation(InData.Location);
	SetActorRotation(InData.Rotation);
}
