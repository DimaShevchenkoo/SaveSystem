// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SS_IDComponent.h"

#if WITH_EDITOR
void USS_IDComponent::PostLoad()
{
	Super::PostLoad();

	if (!ID.IsValid())
	{
		ID = FGuid::NewGuid();
	}
}

void USS_IDComponent::PostEditImport()
{
	Super::PostEditImport();

	ID = FGuid::NewGuid(); // Always generate a fresh one for duplicates
}

void USS_IDComponent::OnRegister()
{
	Super::OnRegister();

	if (!ID.IsValid() && !HasAnyFlags(RF_ClassDefaultObject))
	{
		ID = FGuid::NewGuid();
	}
#endif
}