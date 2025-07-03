// Fill out your copyright notice in the Description page of Project Settings.

#include "Interfaces/SS_SavableInterface.h"
#include "NativeGameplayTags.h"
#include "Components/SS_AutoIDComponent.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Save_ToSpawn, "Save.ToSpawn");
UE_DEFINE_GAMEPLAY_TAG(TAG_Save_Static, "Save.Static");

void ISS_SavableInterface::AssignID()
{
	if (const UObject* Self = Cast<UObject>(this))
	{
		if (const AActor* Actor = Cast<AActor>(Self))
		{
			if (USS_AutoIDComponent* AutoIDComponent = Actor->GetComponentByClass<USS_AutoIDComponent>())
			{
				AutoIDComponent->AssignID();
			}
		}
	}
}
