// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/SS_SavableCharacter.h"

#include "SS_SaveManager.h"
#include "Components/SS_IDComponent.h"
#include "Kismet/GameplayStatics.h"

ASS_SavableCharacter::ASS_SavableCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	ID = CreateDefaultSubobject<USS_IDComponent>("ID");
}

void ASS_SavableCharacter::Save_Implementation(FSaveData& OutData) const
{
	ISS_SavableInterface::Save_Implementation(OutData);

	OutData.Location = GetActorLocation();
	OutData.Rotation = GetActorRotation();
	UE_LOG(LogTemp, Warning, TEXT("ID for Actor: %s"), *GetName());
}

void ASS_SavableCharacter::Load_Implementation(const FSaveData& InData)
{
	ISS_SavableInterface::Load_Implementation(InData);

	SetActorLocation(InData.Location);
	SetActorRotation(InData.Rotation);
}

FGuid ASS_SavableCharacter::GetID_Implementation()
{
	return ID->ID;
}

void ASS_SavableCharacter::Destroyed()
{
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USS_SaveManager>()->AddDestroyedActor(ID->ID);
	Super::Destroyed();
}
