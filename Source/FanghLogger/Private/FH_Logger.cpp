#include "FH_Logger.h"
#include "UObject/Class.h"
#include "Logging/LogMacros.h"

// UObject
void FH_Logger::Log(
	const FLogCategoryBase& Category,
	const EF_LOG Level,
	const UObject* Context,
	const FString& Content,
	const FString& FunctionName)
{
	const FString ObjectName = Context ? Context->GetName() : TEXT("None");
	LogInternal(Category, Level, ObjectName, Content, FunctionName);
}

// UClass
void FH_Logger::Log(
	const FLogCategoryBase& Category,
	const EF_LOG Level,
	const UClass* Context,
	const FString& Content,
	const FString& FunctionName)
{
	const FString ObjectName = Context ? Context->GetName() : TEXT("Class");
	LogInternal(Category, Level, ObjectName, Content, FunctionName);
}

// FString
void FH_Logger::Log(
	const FLogCategoryBase& Category,
	const EF_LOG Level,
	const FString& Context,
	const FString& Content,
	const FString& FunctionName)
{
	LogInternal(Category, Level, Context, Content, FunctionName);
}

// TCHAR*
void FH_Logger::Log(
	const FLogCategoryBase& Category,
	const EF_LOG Level,
	const TCHAR* Context,
	const FString& Content,
	const FString& FunctionName)
{
	LogInternal(
		Category,
		Level,
		Context ? FString(Context) : TEXT("Unknown"),
		Content,
		FunctionName
	);
}

void FH_Logger::LogInternal(
	const FLogCategoryBase& Category,
	const EF_LOG Level,
	const FString& ObjectName,
	const FString& Content,
	const FString& FunctionName)
{
	FString PureFunctionName = FunctionName;
	if (int32 ScopeIndex; PureFunctionName.FindLastChar(TEXT(':'), ScopeIndex))
	{
		PureFunctionName = PureFunctionName.Mid(ScopeIndex + 1);
	}

	const FString Message = FString::Printf( 
		TEXT("[%s::%s] >> %s"),
		*ObjectName,
		*PureFunctionName,
		*Content
	);

	ELogVerbosity::Type Verbosity = ELogVerbosity::Log;
	switch (Level)
	{
	case EF_LOG::Warning: Verbosity = ELogVerbosity::Warning; break;
	case EF_LOG::Error:   Verbosity = ELogVerbosity::Error;   break;
	case EF_LOG::Verbose: Verbosity = ELogVerbosity::Verbose; break;
	default:              Verbosity = ELogVerbosity::Log;     break;
	}

	FMsg::Logf(
		__FILE__,
		__LINE__,
		Category.GetCategoryName(),
		Verbosity,
		TEXT("%s"),
		*Message
	);
}
