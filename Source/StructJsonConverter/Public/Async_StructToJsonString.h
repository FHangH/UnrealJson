#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "StructUtils/InstancedStruct.h"
#include "Async_StructToJsonString.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncStructToJsonSuccess, const FString&, JsonString, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAsyncStructToJsonFailed, const FString&, ErrorMessage);

UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class STRUCTJSONCONVERTER_API UAsync_StructToJsonString : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContext {};
	
	FInstancedStruct SourceStruct {};
	
public:
	UPROPERTY(BlueprintAssignable, DisplayName="Success", Category="FH|UnrealJson|Convert|Async")
	FAsyncStructToJsonSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable, DisplayName="Failed", Category="FH|UnrealJson|Convert|Async")
	FAsyncStructToJsonFailed OnFailed;
	
	/**
	 * 异步 Struct → JsonString
	 * - 该节点用于将 Struct 序列化为 Json 字符串
	 * - 实际序列化工作在非 GameThread 上执行，
	 *   避免复杂或大型 Struct 阻塞主线程
	 * - Struct 使用 FInstancedStruct 作为输入，
	 *   以支持运行时动态 Struct 类型
	 *
	 * 使用方式（Blueprint）：
	 * 1. 使用 MakeInstancedStruct 将任意 Struct 转为 InstancedStruct
	 * 2. 将 InstancedStruct 传入该 Async 节点
	 *
	 * @param WorldContextObject
	 *   世界上下文对象
	 *   - 用于 RegisterWithGameInstance 和 日志打印
	 *
	 * @param Struct
	 *   输入的 InstancedStruct
	 *   - 内部包含 Struct 的 UScriptStruct 类型与实际内存
	 *   - 允许在运行时动态决定 Struct 类型
	 *
	 * @return
	 *   返回异步任务节点实例
	 *   - 实际结果通过 Success / Failed 委托回调
	 */	
	UFUNCTION(BlueprintCallable, DisplayName="Async_StructToJson", Category="FH|UnrealJson|Convert|Async", 
		meta=(BlueprintInternalUseOnly="true", DefaultToSelf="WorldContextObject", AdvancedDisplay="WorldContextObject"))
	static UAsync_StructToJsonString* StructToJsonStringAsync(UObject* WorldContextObject, const FInstancedStruct& Struct);

protected:
	virtual void Activate() override;
	FString GetCallerName() const;
	
public:
	UFUNCTION(BlueprintCallable, Category="FH|UnrealJson|Convert|Async")
	void EndTask();
};
