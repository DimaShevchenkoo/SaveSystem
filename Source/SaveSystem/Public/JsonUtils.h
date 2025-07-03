// All rights reserved by Aboba Inc.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "UObject/Object.h"
#include "JsonUtils.generated.h"

/**
 * 
 */
UCLASS()
class SAVESYSTEM_API UJsonUtils : public UObject
{
	GENERATED_BODY()

public:
	// static void LoadCustomData(AActor* Actor, const FSaveData& Data)
	// {
	// for (TFieldIterator<FProperty> PropIt(Actor->GetClass()); PropIt; ++PropIt)
	// {
	// 	FProperty* Property = *PropIt;
	// 	FString Name = Property->GetName();
	//
	// 	if (!Property->HasAnyPropertyFlags(CPF_SaveGame)) continue;
	// 	// UE_LOG(LogTemp, Warning, TEXT("Property: %s"), *Name);
	// 	// UE_LOG(LogTemp, Warning, TEXT(" - SaveGame"))
	//
	// 	const FString* Value = Data.CustomData.Find(Name);
	// 	if (!Value)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("Missing value for property: %s"), *Property->GetName());
	// 		continue;
	// 	}
	//
	// 	if (const FIntProperty* IntProp = CastField<FIntProperty>(Property))
	// 	{
	// 		IntProp->SetPropertyValue_InContainer(Actor, FCString::Atoi(**Value));
	// 	}
	//
	// 	else if (const FByteProperty* ByteProp = CastField<FByteProperty>(Property))
	// 	{
	// 		ByteProp->SetPropertyValue_InContainer(Actor, static_cast<uint8>(FCString::Atoi(**Value)));
	// 	}
	//
	// 	else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
	// 	{
	// 		FloatProp->SetPropertyValue_InContainer(Actor, FCString::Atof(**Value));
	// 	}
	//
	// 	else if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
	// 	{
	// 		BoolProp->SetPropertyValue_InContainer(Actor, Value->ToBool());
	// 	}
	//
	// 	else if (const FStrProperty* StrProp = CastField<FStrProperty>(Property))
	// 	{
	// 		StrProp->SetPropertyValue_InContainer(Actor, **Value);
	// 	}
	//
	// 	else if (const FTextProperty* TextProp = CastField<FTextProperty>(Property))
	// 	{
	// 		TextProp->SetPropertyValue_InContainer(Actor, FText::FromString(**Value));
	// 	}
	//
	// 	else if (const FNameProperty* NameProp = CastField<FNameProperty>(Property))
	// 	{
	// 		NameProp->SetPropertyValue_InContainer(Actor, FName(**Value));
	// 	}
	//
	// 	// else if (const FObjectProperty* ObjectProp = CastField<FObjectProperty>(Property))
	// 	// {
	// 	// 	// Recursion ???
	// 	// 	ObjectProp->SetPropertyValue_InContainer(Actor, FName(**Value));
	// 	// }
	//
	// 	// else if (const FMapProperty* MapProp = CastField<FMapProperty>(Property))
	// 	// {
	// 	// 	MapProp->SetPropertyValue_InContainer(Actor, FName(**Value));
	// 	// }
	//
	// 	else if (const FStructProperty* StructProp = CastField<FStructProperty>(Property))
	// 	{
	// 		// Step 1: Parse the FString JSON into a JSON object
	// 		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(**Value);
	//
	// 		if (TSharedPtr<FJsonObject> JsonObject; FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	// 		{
	// 			// Step 2: Get pointer to actual struct instance in the actor
	// 			// Step 3: Convert JSON into the struct in-place
	// 			FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructProp->Struct, StructProp->ContainerPtrToValuePtr<void>(Actor), 0,
	// 			                                          0);
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("Invalid JSON string for struct %s:\n%s"), *StructProp->Struct->GetName(), **Value);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("Unknown or unsupported property type: %s"), *Property->GetClass()->GetName());
	// 	}
	// }
	// }

	static TSharedPtr<FJsonObject> GenerateCustomData(const UObject* Container)
	{
		TSharedPtr<FJsonObject> CustomData = MakeShared<FJsonObject>();;
		for (TFieldIterator<FProperty> PropIt(Container->GetClass()); PropIt; ++PropIt)
		{
			const FProperty* Property = *PropIt;
			FString Name = Property->GetName();

			if (!Property->HasAnyPropertyFlags(CPF_SaveGame)) continue;
			// UE_LOG(LogTemp, Warning, TEXT("Property: %s"), *Name);
			// UE_LOG(LogTemp, Warning, TEXT(" - SaveGame"))

			CustomData->SetField(Name, GetJsonValueFromProperty(Container, Property));
		}
		return CustomData;
	}

	static TSharedPtr<FJsonValue> GetJsonValueFromProperty(const void* Container, const FProperty* Property)
	{
		if (const FIntProperty* IntProp = CastField<FIntProperty>(Property))
		{
			return MakeShared<FJsonValueNumber>(IntProp->GetPropertyValue_InContainer(Container));
		}
		if (const FByteProperty* ByteProp = CastField<FByteProperty>(Property))
		{
			if (ByteProp->IsEnum())
			{
				// Not sure if this does anything???
				UE_LOG(LogTemp, Warning, TEXT("GetJsonValueFromProperty ENUM????????????: %s"), *Property->GetCPPType())
				const UEnum* Enum = ByteProp->GetIntPropertyEnum();
				const uint8 Value = ByteProp->GetPropertyValue_InContainer(Container);
				return MakeShared<FJsonValueString>(Enum->GetNameStringByValue(Value));
			}
			return MakeShared<FJsonValueNumber>(ByteProp->GetPropertyValue_InContainer(Container));
		}

		if (const FEnumProperty* EnumProp = CastField<FEnumProperty>(Property))
		{
			const int64 EnumValue = EnumProp->GetUnderlyingProperty()->GetSignedIntPropertyValue(EnumProp->ContainerPtrToValuePtr<void>(Container));
			return MakeShared<FJsonValueString>(EnumProp->GetEnum()->GetNameStringByValue(EnumValue));
		}

		if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
		{
			return MakeShared<FJsonValueNumber>(FloatProp->GetPropertyValue_InContainer(Container));
		}

		if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
		{
			return MakeShared<FJsonValueBoolean>(BoolProp->GetPropertyValue_InContainer(Container));
		}

		if (const FStrProperty* StrProp = CastField<FStrProperty>(Property))
		{
			return MakeShared<FJsonValueString>(StrProp->GetPropertyValue_InContainer(Container));
		}

		if (const FTextProperty* TextProp = CastField<FTextProperty>(Property))
		{
			return MakeShared<FJsonValueString>(TextProp->GetPropertyValue_InContainer(Container).ToString());
		}

		if (const FNameProperty* NameProp = CastField<FNameProperty>(Property))
		{
			return MakeShared<FJsonValueString>(NameProp->GetPropertyValue_InContainer(Container).ToString());
		}

		if (const FMapProperty* MapProp = CastField<FMapProperty>(Property))
		{
			const TSharedPtr<FJsonObject> MapObject = MakeShared<FJsonObject>();
			const void* Map = MapProp->ContainerPtrToValuePtr<void>(Container);

			FScriptMapHelper Helper(MapProp, Map);
			for (int32 i = 0; i < Helper.GetMaxIndex(); ++i)
			{
				if (!Helper.IsValidIndex(i))
					continue;

				const void* KeyPtr = Helper.GetKeyPtr(i);
				const void* ValuePtr = Helper.GetValuePtr(i);

				const FString KeyStr = GetPropertyAsString(MapProp->KeyProp, KeyPtr);
				const TSharedPtr<FJsonValue> JsonValue = GetJsonValueFromDirectPtr(ValuePtr, MapProp->ValueProp);

				MapObject->SetField(KeyStr, JsonValue);
			}
			return MakeShared<FJsonValueObject>(MapObject);
		}

		if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
		{
			TArray<TSharedPtr<FJsonValue>> JsonArray;
			const void* ArrayPtr = ArrayProp->ContainerPtrToValuePtr<void>(Container);
			FScriptArrayHelper Helper(ArrayProp, ArrayPtr);
			for (int32 i = 0; i < Helper.Num(); ++i)
			{
				const void* ElemPtr = Helper.GetRawPtr(i);
				const FProperty* InnerProp = ArrayProp->Inner;
				JsonArray.Add(GetJsonValueFromProperty(ElemPtr, InnerProp));
			}
			return MakeShared<FJsonValueArray>(JsonArray);
		}
		if (const FStructProperty* StructProp = CastField<FStructProperty>(Property))
		{
			TSharedPtr<FJsonObject> StructJson = MakeShared<FJsonObject>();
			FJsonObjectConverter::UStructToJsonObject(StructProp->Struct, StructProp->ContainerPtrToValuePtr<void>(Container), StructJson.ToSharedRef());
			return MakeShared<FJsonValueObject>(StructJson);
		}
		if (const FObjectProperty* ObjectProp = CastField<FObjectProperty>(Property))
		{
			if (const UObject* ObjectValue = ObjectProp->GetObjectPropertyValue_InContainer(Container))
			{
				return MakeShared<FJsonValueObject>(GenerateCustomData(ObjectValue));
			}
			return MakeShared<FJsonValueNull>(); // handle nullptr safely
		}


		UE_LOG(LogTemp, Warning, TEXT("Unknown save type: %s"), *Property->GetCPPType())
		return MakeShared<FJsonValueNull>();
	}

	static TSharedPtr<FJsonValue> GetJsonValueFromDirectPtr(const void* ValuePtr, const FProperty* Property)
	{
		if (const FIntProperty* IntProp = CastField<FIntProperty>(Property))
		{
			return MakeShared<FJsonValueNumber>(IntProp->GetPropertyValue(ValuePtr));
		}
		
		if (const FByteProperty* ByteProp = CastField<FByteProperty>(Property))
		{
			if (ByteProp->IsEnum())
			{
				// Not sure if this does anything???
				UE_LOG(LogTemp, Warning, TEXT("GetJsonValueFromProperty ENUM????????????: %s"), *Property->GetCPPType())
				const UEnum* Enum = ByteProp->GetIntPropertyEnum();
				const uint8 Value = ByteProp->GetPropertyValue(ValuePtr);
				return MakeShared<FJsonValueString>(Enum->GetNameStringByValue(Value));
			}
			return MakeShared<FJsonValueNumber>(ByteProp->GetPropertyValue(ValuePtr));
		}

		if (const FEnumProperty* EnumProp = CastField<FEnumProperty>(Property))
		{
			const int64 EnumValue = EnumProp->GetUnderlyingProperty()->GetSignedIntPropertyValue(ValuePtr);
			return MakeShared<FJsonValueString>(EnumProp->GetEnum()->GetNameStringByValue(EnumValue));
		}

		if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
		{
			return MakeShared<FJsonValueNumber>(FloatProp->GetPropertyValue(ValuePtr));
		}

		if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
		{
			return MakeShared<FJsonValueBoolean>(BoolProp->GetPropertyValue(ValuePtr));
		}

		if (const FStrProperty* StrProp = CastField<FStrProperty>(Property))
		{
			return MakeShared<FJsonValueString>(StrProp->GetPropertyValue(ValuePtr));
		}

		if (const FTextProperty* TextProp = CastField<FTextProperty>(Property))
		{
			return MakeShared<FJsonValueString>(TextProp->GetPropertyValue(ValuePtr).ToString());
		}

		if (const FNameProperty* NameProp = CastField<FNameProperty>(Property))
		{
			return MakeShared<FJsonValueString>(NameProp->GetPropertyValue(ValuePtr).ToString());
		}

		// Unsupported key type
		UE_LOG(LogTemp, Warning, TEXT("Unsupported value, %p"), ValuePtr);
		return MakeShared<FJsonValueNull>();
	}

	static FString GetPropertyAsString(const FProperty* Prop, const void* Ptr)
	{
		if (const FStrProperty* StrKeyProp = CastField<FStrProperty>(Prop))
		{
			return StrKeyProp->GetPropertyValue(Ptr);
		}
		if (const FNameProperty* NameKeyProp = CastField<FNameProperty>(Prop))
		{
			return NameKeyProp->GetPropertyValue(Ptr).ToString();
		}
		if (const FTextProperty* TextKeyProp = CastField<FTextProperty>(Prop))
		{
			return TextKeyProp->GetPropertyValue(Ptr).ToString();
		}
		if (const FByteProperty* ByteKeyProp = CastField<FByteProperty>(Prop))
		{
			if (ByteKeyProp->IsEnum())
			{
				const uint8 EnumValue = ByteKeyProp->GetPropertyValue(Ptr);
				return ByteKeyProp->GetIntPropertyEnum()->GetNameStringByValue(EnumValue);
			}
			return FString::Printf(TEXT("%u"), ByteKeyProp->GetPropertyValue(Ptr));
		}
		if (const FEnumProperty* EnumKeyProp = CastField<FEnumProperty>(Prop))
		{
			int64 EnumValue = EnumKeyProp->GetUnderlyingProperty()->GetSignedIntPropertyValue(Ptr);
			return EnumKeyProp->GetEnum()->GetNameStringByValue(EnumValue);
		}
		if (const FIntProperty* IntKeyProp = CastField<FIntProperty>(Prop))
		{
			return FString::FromInt(IntKeyProp->GetPropertyValue(Ptr));
		}

		// Unsupported key type
		UE_LOG(LogTemp, Warning, TEXT("Unsupported map key type: %s"), *Prop->GetCPPType());
		return TEXT("UnsupportedKey");
	}
};
