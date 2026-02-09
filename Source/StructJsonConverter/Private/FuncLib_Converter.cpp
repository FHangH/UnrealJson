#include "FuncLib_Converter.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/OutputDevice.h"
#include "InstancedStruct.h"
#include "FanghLogger/Public/FanghLoggerMacros.h"

void UFuncLib_Converter::BP_StructToJsonString(
	const int32& Struct, FString& OutJsonString, EJsonConvertResult& Result, FString& ErrorMessage)
{
	Result = EJsonConvertResult::Failed;
	ErrorMessage = TEXT("Invalid call");
}

DEFINE_FUNCTION(UFuncLib_Converter::execBP_StructToJsonString)
{
	FH_LOG(LogUnrealJsonConvert, EF_LOG::Log, StaticClass(), TEXT("Start BP_StructToJsonString"));
	
	// 取结构体参数（Wildcard）
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	const auto* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	const void* StructData = Stack.MostRecentPropertyAddress;

	// 取输出参数
	P_GET_PROPERTY_REF(FStrProperty, OutJsonString);
	P_GET_ENUM_REF(EJsonConvertResult, Result);
	P_GET_PROPERTY_REF(FStrProperty, ErrorMessage);
	P_FINISH;

	if (!StructProp || !StructData)
	{
		Result = EJsonConvertResult::Failed;
		ErrorMessage = TEXT("Invalid Struct");
		FH_LOG(LogUnrealJsonConvert, EF_LOG::Error, StaticClass(), TEXT("Invalid Struct"));
		return;
	}
	
	if (!FJsonObjectConverter::UStructToJsonObjectString(StructProp->Struct, StructData, OutJsonString))
	{
		Result = EJsonConvertResult::Failed;
		ErrorMessage = TEXT("Struct to Json failed");
		FH_LOG(LogUnrealJsonConvert, EF_LOG::Log, StaticClass(), TEXT("Struct to Json failed"));
		return;
	}

	Result = EJsonConvertResult::Success;
	ErrorMessage.Empty();
	FH_LOG(LogUnrealJsonConvert, EF_LOG::Log, StaticClass(), TEXT("End BP_StructToJsonString"));
}

void UFuncLib_Converter::BP_JsonStringToStruct(
	const FString& JsonString, int32& Struct, EJsonConvertResult& Result, FString& ErrorMessage)
{
	Result = EJsonConvertResult::Failed;
	ErrorMessage = TEXT("Invalid call");
}

DEFINE_FUNCTION(UFuncLib_Converter::execBP_JsonStringToStruct)
{
	FH_LOG(LogUnrealJsonConvert, EF_LOG::Log, StaticClass(), TEXT("Start BP_JsonStringToStruct"));

	// 取输出参数
	P_GET_PROPERTY(FStrProperty, JsonString);
	
	// 取结构体参数（Wildcard）
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	const FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructData = Stack.MostRecentPropertyAddress;
	
	P_GET_ENUM_REF(EJsonConvertResult, Result);
	P_GET_PROPERTY_REF(FStrProperty, ErrorMessage);
	P_FINISH;

	if (!StructProp || !StructData || JsonString.IsEmpty())
	{
		Result = EJsonConvertResult::Failed;
		ErrorMessage = TEXT("Invalid Struct");
		FH_LOG(LogUnrealJsonConvert, EF_LOG::Error, StaticClass(), TEXT("Invalid Struct"));
		return;
	}
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		Result = EJsonConvertResult::Failed;
		ErrorMessage = TEXT("JsonString Deserialize failed");
		FH_LOG(LogUnrealJsonConvert, EF_LOG::Error, StaticClass(), TEXT("JsonString Deserialize failed"));
		return;
	}
	
	if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructProp->Struct, StructData))
	{
		Result = EJsonConvertResult::Success;
        ErrorMessage.Empty();
		FH_LOG(LogUnrealJsonConvert, EF_LOG::Log, StaticClass(), TEXT("End BP_JsonStringToStruct"));
		return;
	}
	Result = EJsonConvertResult::Failed;
	ErrorMessage = TEXT("JsonString to Struct failed");
	FH_LOG(LogUnrealJsonConvert, EF_LOG::Error, StaticClass(), TEXT("JsonString to Struct failed"));
}