#include "UnrealJsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"

typedef TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> FCondensedJsonStringWriterFactory;

UUnrealJsonObject::UUnrealJsonObject() : Super()
{
	Reset();
}

void UUnrealJsonObject::Reset()
{
	if (JsonObj.IsValid())
	{
		JsonObj.Reset();
	}

	JsonObj = MakeShareable(new FJsonObject());
}

TSharedPtr<FJsonObject>& UUnrealJsonObject::GetRootObject()
{
	return JsonObj;
}

void UUnrealJsonObject::SetRootObject(const TSharedPtr<FJsonObject>& JsonObject)
{
	JsonObj = JsonObject;
}

UUnrealJsonObject* UUnrealJsonObject::ConstructUnrealJsonObject(UObject* WorldContextObject)
{
	return NewObject<UUnrealJsonObject>();
}

void UUnrealJsonObject::SetStringField(const FString& FieldName, const FString& StringValue) const
{
	if (FieldName.IsEmpty())
	{
		return;
	}

	JsonObj->SetStringField(FieldName, StringValue);
}

void UUnrealJsonObject::SetBoolField(const FString& FieldName, bool InValue) const
{
	if (FieldName.IsEmpty())
	{
		return;
	}

	JsonObj->SetBoolField(FieldName, InValue);
}

void UUnrealJsonObject::SetNumberField(const FString& FieldName, double Number) const
{
	if (FieldName.IsEmpty())
	{
		return;
	}

	JsonObj->SetNumberField(FieldName, Number);
}

void UUnrealJsonObject::SetObjectField(const FString& FieldName, UUnrealJsonObject* JsonObject) const
{
	if (FieldName.IsEmpty())
	{
		return;
	}

	if (JsonObject && JsonObject->GetRootObject().IsValid())
	{
		JsonObj->SetObjectField(FieldName, JsonObject->GetRootObject());
	}
}

void UUnrealJsonObject::SetStringArrayField(const FString& FieldName, const TArray<FString>& StringArray) const
{
	if (FieldName.IsEmpty())
	{
		return;
	}

	TArray<TSharedPtr<FJsonValue>> JsonValues;
	for (const FString& Str : StringArray)
	{
		JsonValues.Add(MakeShareable(new FJsonValueString(Str)));
	}

	JsonObj->SetArrayField(FieldName, JsonValues);
}

void UUnrealJsonObject::SetBoolArrayField(const FString& FieldName, const TArray<bool>& BoolArray) const
{
	if (FieldName.IsEmpty())
	{
		return;
	}

	TArray<TSharedPtr<FJsonValue>> JsonValues;
	for (const bool& Val : BoolArray)
	{
		JsonValues.Add(MakeShareable(new FJsonValueBoolean(Val)));
	}

	JsonObj->SetArrayField(FieldName, JsonValues);
}

void UUnrealJsonObject::SetNumberArrayField(const FString& FieldName, const TArray<double>& NumberArray) const
{
	if (FieldName.IsEmpty())
	{
		return;
	}

	TArray<TSharedPtr<FJsonValue>> JsonValues;
	for (const double& Val : NumberArray)
	{
		JsonValues.Add(MakeShareable(new FJsonValueNumber(Val)));
	}

	JsonObj->SetArrayField(FieldName, JsonValues);
}

void UUnrealJsonObject::SetObjectArrayField(const FString& FieldName, const TArray<UUnrealJsonObject*>& ObjectArray) const
{
	if (FieldName.IsEmpty())
	{
		return;
	}

	TArray<TSharedPtr<FJsonValue>> JsonValues;
	for (UUnrealJsonObject* Item : ObjectArray)
	{
		if (Item && Item->GetRootObject().IsValid())
		{
			JsonValues.Add(MakeShareable(new FJsonValueObject(Item->GetRootObject())));
		}
	}

	JsonObj->SetArrayField(FieldName, JsonValues);
}

FString UUnrealJsonObject::EncodeJson() const
{
	if (!JsonObj.IsValid())
	{
		return TEXT("");
	}

	FString OutputString;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);

	return OutputString;
}

FString UUnrealJsonObject::EncodeJsonToSingleString() const
{
	if (!JsonObj.IsValid())
	{
		return TEXT("");
	}
	
	FString OutputString;
	const auto Writer = FCondensedJsonStringWriterFactory::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObj, Writer);

	return OutputString;
}
