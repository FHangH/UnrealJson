#include "Async_JsonStringToStruct.h"
#include "JsonObjectConverter.h"
#include "StructUtils/InstancedStruct.h"
#include "FanghLogger/Public/FanghLoggerMacros.h"

UAsync_JsonStringToStruct* UAsync_JsonStringToStruct::JsonStringToStructAsync(
	UObject* WorldContextObject, const FString& JsonString, UScriptStruct* StructType)
{
	UAsync_JsonStringToStruct* Node = NewObject<UAsync_JsonStringToStruct>();
	Node->WorldContext = WorldContextObject;
	Node->Json = JsonString;
	Node->TargetStruct = StructType;
	return Node;
}

void UAsync_JsonStringToStruct::Activate()
{
	FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Log, GetCallerName(), TEXT("UAsync_JsonStringToStruct->Activate"), TEXT("Begin Task"));
	RegisterWithGameInstance(WorldContext);
	
	if (!TargetStruct)
	{
		OnFailed.Broadcast(TEXT("Invalid StructType"));
		FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Warning, GetCallerName(), TEXT("UAsync_JsonStringToStruct->Activate"), TEXT("Invalid StructType"));
		EndTask();
		return;
	}

	Async(EAsyncExecution::ThreadPool, [this]()
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);

		if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
		{
			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				OnFailed.Broadcast(TEXT("JsonString Deserialize failed"));
				FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Error, GetCallerName(), TEXT("UAsync_JsonStringToStruct->Activate"), TEXT("JsonString Deserialize failed"));
			});
			return;
		}

		FInstancedStruct Result;
		Result.InitializeAs(TargetStruct);

		if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), TargetStruct, Result.GetMutableMemory()))
		{
			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				OnFailed.Broadcast(TEXT("JsonObject to Struct failed"));
				FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Error, GetCallerName(), TEXT("UAsync_JsonStringToStruct->Activate"), TEXT("JsonObject to Struct failed"));
			});
			return;
		}

		AsyncTask(ENamedThreads::GameThread, [this, Result]()
		{
			OnSuccess.Broadcast(Result, TEXT(""));
			FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Log, GetCallerName(), TEXT("UAsync_JsonStringToStruct->Activate"), TEXT("Success"));
		});
	});
	EndTask();
}

FString UAsync_JsonStringToStruct::GetCallerName() const
{
	return WorldContext ? WorldContext->GetName() : TEXT("Unknown");
}

void UAsync_JsonStringToStruct::EndTask()
{
	SetReadyToDestroy();
	MarkAsGarbage();
	FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Log, GetCallerName(), TEXT("UAsync_JsonStringToStruct->EndTask"), TEXT("End Task"));
}
