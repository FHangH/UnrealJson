#pragma once

#include "CoreMinimal.h"
#include "UnrealMakeJsonTypes.generated.h"

UENUM(BlueprintType)
enum class EUnrealJson_JsonType : uint8
{
	JSON_Bool UMETA(DisplayName = "Boolean"),
	JSON_Number UMETA(DisplayName = "Number"),
	JSON_String UMETA(DisplayName = "String"),
	JSON_Object UMETA(DisplayName = "Object")
};

USTRUCT(BlueprintType)
struct FUnrealJson_NamedType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = NamedType)
	FName Name;

	UPROPERTY(EditAnywhere, Category = NamedType)
	EUnrealJson_JsonType Type;

	UPROPERTY(EditAnywhere, Category = NamedType)
	bool bIsArray;

	FUnrealJson_NamedType()
		: Type(EUnrealJson_JsonType::JSON_String)
		, bIsArray(false)
	{
	}
};
