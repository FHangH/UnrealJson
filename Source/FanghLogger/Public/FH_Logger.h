#pragma once

#include "CoreMinimal.h"

enum class EF_LOG : uint8
{
	Log,
	Warning,
	Error,
	Verbose
};

class FANGHLOGGER_API FH_Logger
{
public:
	// UObject / Actor / Component / Subsystem / BP
	static void Log(
		const FLogCategoryBase& Category,
		EF_LOG Level,
		const UObject* Context,
		const FString& Content,
		const FString& FunctionName
	);

	// UClass（FunctionLibrary / StaticClass）
	static void Log(
		const FLogCategoryBase& Category,
		EF_LOG Level,
		const UClass* Context,
		const FString& Content,
		const FString& FunctionName
	);

	// FString
	static void Log(
		const FLogCategoryBase& Category,
		EF_LOG Level,
		const FString& Context,
		const FString& Content,
		const FString& FunctionName
	);

	// TCHAR*（字符串字面量）
	static void Log(
		const FLogCategoryBase& Category,
		EF_LOG Level,
		const TCHAR* Context,
		const FString& Content,
		const FString& FunctionName
	);

private:
	static void LogInternal(
		const FLogCategoryBase& Category,
		EF_LOG Level,
		const FString& ObjectName,
		const FString& Content,
		const FString& FunctionName
	);
};
