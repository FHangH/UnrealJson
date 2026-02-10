#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "UnrealJsonObject.generated.h"

/**
 * Wrapper class for FJsonObject to be used in Blueprints
 */
UCLASS(BlueprintType, Blueprintable)
class UNREALMAKEJSON_API UUnrealJsonObject : public UObject
{
	GENERATED_BODY()

public:
	UUnrealJsonObject();

	/** Reset all internal data */
	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson")
	void Reset();

	/** Get the underlying Json Object */
	TSharedPtr<FJsonObject>& GetRootObject();

	/** Set the underlying Json Object */
	void SetRootObject(const TSharedPtr<FJsonObject>& JsonObject);

	/** Construct a new UnrealJsonObject */
	UFUNCTION(BlueprintCallable, DisplayName="ConstructJsonObject", Category = "FH|UnrealJson|MakeJson",
		meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UUnrealJsonObject* ConstructUnrealJsonObject(UObject* WorldContextObject);

	//////////////////////////////////////////////////////////////////////////
	// FJsonObject API

	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson|Set")
	void SetStringField(const FString& FieldName, const FString& StringValue) const;

	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson|Set")
	void SetBoolField(const FString& FieldName, bool InValue) const;

	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson|Set")
	void SetNumberField(const FString& FieldName, double Number) const;

	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson|Set")
	void SetObjectField(const FString& FieldName, UUnrealJsonObject* JsonObject) const;

	// Array support
	
	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson|Set")
	void SetStringArrayField(const FString& FieldName, const TArray<FString>& StringArray) const;

	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson|Set")
	void SetBoolArrayField(const FString& FieldName, const TArray<bool>& BoolArray) const;

	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson|Set")
	void SetNumberArrayField(const FString& FieldName, const TArray<double>& NumberArray) const;

	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson|Set")
	void SetObjectArrayField(const FString& FieldName, const TArray<UUnrealJsonObject*>& ObjectArray) const;

	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson|Encode")
	FString EncodeJson() const;
	
	UFUNCTION(BlueprintCallable, Category = "FH|UnrealJson|MakeJson|Encode")
	FString EncodeJsonToSingleString() const;

protected:
	TSharedPtr<FJsonObject> JsonObj;
};
