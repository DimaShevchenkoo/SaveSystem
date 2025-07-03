// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/SS_SavableActor.h"
#include "Components/SS_AutoIDComponent.h"

ASS_SavableActor::ASS_SavableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoIDComponent = CreateDefaultSubobject<USS_AutoIDComponent>("AutoIDComponent");
}

void ASS_SavableActor::Save_Implementation(FSaveData& OutData) const
{
	ISS_SavableInterface::Save_Implementation(OutData);

	OutData.Location = GetActorLocation();
	OutData.Rotation = GetActorRotation();
	OutData.ActorID = AutoIDComponent->ActorID;
}

void ASS_SavableActor::Load_Implementation(const FSaveData& InData)
{
	ISS_SavableInterface::Load_Implementation(InData);

	SetActorLocation(InData.Location);
	SetActorRotation(InData.Rotation);
}
