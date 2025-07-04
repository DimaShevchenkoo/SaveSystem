// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/SS_SavableActor.h"

#include "SS_SaveManager.h"
#include "Components/SS_IDComponent.h"
#include "Kismet/GameplayStatics.h"

ASS_SavableActor::ASS_SavableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	IDComponent = CreateDefaultSubobject<USS_IDComponent>("ID");
}

void ASS_SavableActor::Save_Implementation(FSaveData& OutData) const
{
	ISS_SavableInterface::Save_Implementation(OutData);

	OutData.Location = GetActorLocation();
	OutData.Rotation = GetActorRotation();
}

void ASS_SavableActor::Load_Implementation(const FSaveData& InData)
{
	ISS_SavableInterface::Load_Implementation(InData);

	SetActorLocation(InData.Location);
	SetActorRotation(InData.Rotation);
}

void ASS_SavableActor::Destroyed()
{
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USS_SaveManager>()->AddDestroyedActor(IDComponent->ID);
	Super::Destroyed();
}

FGuid ASS_SavableActor::GetID_Implementation()
{
	return IDComponent->ID;
}
