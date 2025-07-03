// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SS_AutoIDComponent.h"

void USS_AutoIDComponent::OnRegister()
{
	Super::OnRegister();

	if (ActorID.IsEmpty())
	{
		ActorID = FGuid::NewGuid().ToString(EGuidFormats::Digits);
	}
}
