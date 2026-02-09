#pragma once

#include "CoreMinimal.h"
#include "JsonConvertTypes.generated.h"

UENUM(BlueprintType)
enum class EJsonConvertResult : uint8
{
	Success UMETA(DisplayName = "Success"),
	Failed  UMETA(DisplayName = "Failed")
};
