#include "Async_StructToJsonString.h"
#include "JsonObjectConverter.h"
#include "FanghLogger/Public/FanghLoggerMacros.h"

UAsync_StructToJsonString* UAsync_StructToJsonString::StructToJsonStringAsync(UObject* WorldContextObject, const FInstancedStruct& Struct)
{
	UAsync_StructToJsonString* Node = NewObject<UAsync_StructToJsonString>();
	Node->WorldContext = WorldContextObject;
	Node->SourceStruct = Struct; 
	return Node;
}

void UAsync_StructToJsonString::Activate()
{
	FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Log, GetCallerName(), TEXT("UAsync_StructToJsonString->Activate"), TEXT("Begin Task"));
	RegisterWithGameInstance(WorldContext);
	
	if (!SourceStruct.IsValid())
	{
		OnFailed.Broadcast(TEXT("Invalid InstancedStruct"));
		FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Warning, GetCallerName(), TEXT("UAsync_StructToJsonString->Activate"), TEXT("Invalid InstancedStruct"));
		EndTask();
		return;
	}

	Async(EAsyncExecution::ThreadPool, [this]()
	{
		FString JsonString;

		if (!FJsonObjectConverter::UStructToJsonObjectString(SourceStruct.GetScriptStruct(), SourceStruct.GetMemory(), JsonString))
		{
			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				OnFailed.Broadcast(TEXT("Struct to Json failed"));
				FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Error, GetCallerName(), TEXT("UAsync_StructToJsonString->Activate"), TEXT("Struct to Json failed"));
			});
			return;
		}

		AsyncTask(ENamedThreads::GameThread, [this, JsonString]()
		{
			OnSuccess.Broadcast(JsonString, TEXT(""));
			FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Log, GetCallerName(), TEXT("UAsync_StructToJsonString->Activate"), TEXT("Struct to Json success"));
		});
	});
	EndTask();
}

FString UAsync_StructToJsonString::GetCallerName() const
{
	return WorldContext ? WorldContext->GetName() : TEXT("Unknown");
}

void UAsync_StructToJsonString::EndTask()
{
	SetReadyToDestroy();
	MarkAsGarbage();
	FH_LOG_F(LogUnrealJsonConvert, EF_LOG::Log, GetCallerName(), TEXT("UAsync_StructToJsonString->EndTask"), TEXT("End Task"));
}