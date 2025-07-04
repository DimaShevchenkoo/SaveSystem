// All rights reserved by Aboba Inc.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Interfaces/SS_SavableInterface.h"
#include "UObject/Object.h"
#include "JsonUtils.generated.h"

UCLASS()
class SAVESYSTEM_API UJsonUtils : public UObject
{
	GENERATED_BODY()

public:
	static void LoadJsonValueFromProperty(const TSharedPtr<FJsonObject>& JsonObject, void* Target,
	                                      FProperty* Property, const bool bToDirectPtr = false)
	{
		const FString PropertyName = Property->GetName();
		if (const FIntProperty* IntProp = CastField<FIntProperty>(Property))
		{
			int32 Number;
			JsonObject->TryGetNumberField(PropertyName, Number);
			bToDirectPtr
				? IntProp->SetPropertyValue(Target, Number)
				: IntProp->SetPropertyValue_InContainer(Target, Number);
		}

		else if (const FByteProperty* ByteProp = CastField<FByteProperty>(Property))
		{
			uint8 Byte;
			JsonObject->TryGetNumberField(PropertyName, Byte);
			bToDirectPtr
				? ByteProp->SetPropertyValue(Target, Byte)
				: ByteProp->SetPropertyValue_InContainer(Target, Byte);
		}

		else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
		{
			float Floaty;
			JsonObject->TryGetNumberField(PropertyName, Floaty);
			bToDirectPtr
				? FloatProp->SetPropertyValue(Target, Floaty)
				: FloatProp->SetPropertyValue_InContainer(Target, Floaty);
		}

		else if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
		{
			bool Bool;
			JsonObject->TryGetBoolField(PropertyName, Bool);
			bToDirectPtr
				? BoolProp->SetPropertyValue(Target, Bool)
				: BoolProp->SetPropertyValue_InContainer(Target, Bool);
		}

		else if (const FStrProperty* StrProp = CastField<FStrProperty>(Property))
		{
			FString String;
			JsonObject->TryGetStringField(PropertyName, String);
			bToDirectPtr
				? StrProp->SetPropertyValue(Target, String)
				: StrProp->SetPropertyValue_InContainer(Target, String);
		}

		else if (const FTextProperty* TextProp = CastField<FTextProperty>(Property))
		{
			FString Text;
			JsonObject->TryGetStringField(PropertyName, Text);
			bToDirectPtr ? TextProp->SetPropertyValue(Target, FText::FromString(Text)) : TextProp->SetPropertyValue_InContainer(Target, FText::FromString(Text));
		}

		else if (const FNameProperty* NameProp = CastField<FNameProperty>(Property))
		{
			FString Name;
			JsonObject->TryGetStringField(PropertyName, Name);
			bToDirectPtr ? NameProp->SetPropertyValue(Target, FName(*Name)) : NameProp->SetPropertyValue_InContainer(Target, FName(*Name));
		}

		else if (const FObjectProperty* ObjectProp = CastField<FObjectProperty>(Property))
		{
			if (const TSharedPtr<FJsonObject>* ObjectValue; JsonObject->TryGetObjectField(PropertyName, ObjectValue))
			{
				UObject* ObjectInstance = ObjectProp->GetObjectPropertyValue_InContainer(Target);
				if (!ObjectInstance)
				{
					UE_LOG(LogTemp, Warning, TEXT("ObjectProperty is null for %s"), *PropertyName);
					return;
				}
				for (TFieldIterator<FProperty> It(ObjectInstance->GetClass()); It; ++It)
				{
					LoadJsonValueFromProperty(*ObjectValue, ObjectInstance, *It);
				}
			}
		}

		else if (const FSetProperty* SetProp = CastField<FSetProperty>(Property))
		{
			// TODO
		}

		else if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
		{
			// TODO
		}

		else if (const FMapProperty* MapProp = CastField<FMapProperty>(Property))
		{
			// TODO
		}

		else if (const FStructProperty* StructProp = CastField<FStructProperty>(Property))
		{
			// TODO
		}
	}

	static void LoadCustomData(AActor* Actor, const FSaveData& Data)
	{
		TSharedPtr<FJsonObject> JsonObject;

		// Parse JSON string from Data.CustomData
		if (const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Data.CustomData); !FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON from CustomData"));
			return;
		}
		
		for (TFieldIterator<FProperty> PropIt(Actor->GetClass()); PropIt; ++PropIt)
		{
			FProperty* Property = *PropIt;
			if (!Property->HasAnyPropertyFlags(CPF_SaveGame)) continue;
			
			FString PropertyName = Property->GetName();
			

			LoadJsonValueFromProperty(JsonObject, Actor, Property);
		}
	}

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

	template <class T = FJsonValue, class Property = FProperty>
	static TSharedPtr<FJsonValue> MakeJsonValueFrom(const void* Container, const Property* TargetProperty,
	                                                const bool bFromDirectPtr = false)
	{
		return bFromDirectPtr
			       ? MakeShared<T>(TargetProperty->GetPropertyValue(Container))
			       : MakeShared<T>(TargetProperty->GetPropertyValue_InContainer(Container));
	}

	static TSharedPtr<FJsonValue> GetJsonValueFromProperty(const void* Container, const FProperty* Property,
	                                                       const bool bFromDirectPtr = false)
	{
		if (const FIntProperty* IntProp = CastField<FIntProperty>(Property))
		{
			return MakeJsonValueFrom<FJsonValueNumber, FIntProperty>(Container, IntProp, bFromDirectPtr);
		}
		if (const FByteProperty* ByteProp = CastField<FByteProperty>(Property))
		{
			if (ByteProp->IsEnum())
			{
				const UEnum* Enum = ByteProp->GetIntPropertyEnum();
				const uint8 Value = bFromDirectPtr
					                    ? ByteProp->GetPropertyValue(Container)
					                    : ByteProp->GetPropertyValue_InContainer(Container);
				return MakeShared<FJsonValueString>(Enum->GetNameStringByValue(Value));
			}
			return bFromDirectPtr
				       ? MakeShared<FJsonValueNumber>(ByteProp->GetPropertyValue(Container))
				       : MakeShared<FJsonValueNumber>(ByteProp->GetPropertyValue_InContainer(Container));
		}

		if (const FEnumProperty* EnumProp = CastField<FEnumProperty>(Property))
		{
			const int64 EnumValue = bFromDirectPtr
				                        ? EnumProp->GetUnderlyingProperty()->GetSignedIntPropertyValue(Container)
				                        : EnumProp->GetUnderlyingProperty()->GetSignedIntPropertyValue(
					                        EnumProp->ContainerPtrToValuePtr<void>(Container));

			return MakeShared<FJsonValueString>(EnumProp->GetEnum()->GetNameStringByValue(EnumValue));
		}

		if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
		{
			return MakeJsonValueFrom<FJsonValueNumber, FFloatProperty>(Container, FloatProp, bFromDirectPtr);
		}

		if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
		{
			return MakeJsonValueFrom<FJsonValueBoolean, FBoolProperty>(Container, BoolProp, bFromDirectPtr);
		}

		if (const FStrProperty* StrProp = CastField<FStrProperty>(Property))
		{
			FString Value = bFromDirectPtr
				                ? StrProp->GetPropertyValue(Container)
				                : StrProp->GetPropertyValue_InContainer(Container);
			return MakeShared<FJsonValueString>(Value);
		}

		if (const FTextProperty* TextProp = CastField<FTextProperty>(Property))
		{
			const FText Value = bFromDirectPtr
				                    ? TextProp->GetPropertyValue(Container)
				                    : TextProp->GetPropertyValue_InContainer(Container);
			return MakeShared<FJsonValueString>(Value.ToString());
		}

		if (const FNameProperty* NameProp = CastField<FNameProperty>(Property))
		{
			const FName Value = bFromDirectPtr
				                    ? NameProp->GetPropertyValue(Container)
				                    : NameProp->GetPropertyValue_InContainer(Container);
			return MakeShared<FJsonValueString>(Value.ToString());
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
				const TSharedPtr<FJsonValue> JsonValue = GetJsonValueFromProperty(ValuePtr, MapProp->ValueProp, true);

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
			FJsonObjectConverter::UStructToJsonObject(StructProp->Struct,
			                                          StructProp->ContainerPtrToValuePtr<void>(Container),
			                                          StructJson.ToSharedRef());
			return MakeShared<FJsonValueObject>(StructJson);
		}
		if (const FObjectProperty* ObjectProp = CastField<FObjectProperty>(Property))
		{
			if (const UObject* ObjectValue = bFromDirectPtr
				                                 ? ObjectProp->GetObjectPropertyValue(Container)
				                                 : ObjectProp->GetObjectPropertyValue_InContainer(Container))
			{
				return MakeShared<FJsonValueObject>(GenerateCustomData(ObjectValue));
			}
			return MakeShared<FJsonValueNull>();
		}

		if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
		{
			TArray<TSharedPtr<FJsonValue>> JsonArray;

			const void* SetPtr = SetProperty->ContainerPtrToValuePtr<void>(Container);
			FScriptSetHelper SetHelper(SetProperty, SetPtr);

			for (int32 i = 0; i < SetHelper.GetMaxIndex(); ++i)
			{
				if (!SetHelper.IsValidIndex(i))
					continue;

				const void* ElemPtr = SetHelper.GetElementPtr(i);
				const FProperty* ElemProp = SetProperty->ElementProp;

				TSharedPtr<FJsonValue> JsonValue = GetJsonValueFromProperty(ElemPtr, ElemProp);
				JsonArray.Add(JsonValue);
			}

			return MakeShared<FJsonValueArray>(JsonArray);
		}

		/* Consider adding FClassProperty */
		// - No

		//if (const FClassProperty) 
		{
		}

		UE_LOG(LogTemp, Warning, TEXT("Unknown save type: %s"), *Property->GetCPPType())
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
