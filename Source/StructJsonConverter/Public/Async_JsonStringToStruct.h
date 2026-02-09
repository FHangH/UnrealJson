#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Async_JsonStringToStruct.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncJsonToStructSuccess, const FInstancedStruct&, ResultStruct, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAsyncJsonToStructFailed, const FString&, ErrorMessage);

UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class STRUCTJSONCONVERTER_API UAsync_JsonStringToStruct : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContext {};
	
	FString Json {};
	
	UPROPERTY()
	UScriptStruct* TargetStruct { nullptr };
	
public:
	UPROPERTY(BlueprintAssignable, DisplayName="Success", Category="FH|UnrealJson|Convert|Async")
	FAsyncJsonToStructSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable, DisplayName="Failed", Category="FH|UnrealJson|Convert|Async")
	FAsyncJsonToStructFailed OnFailed;
	
	/**
	 * 异步 JsonString → Struct
	 * - 该节点用于在非 GameThread 上执行 Json 反序列化
	 * - 适用于 Json 内容较大或结构复杂，避免阻塞主线程
	 * - Struct 类型由调用方显式指定（UScriptStruct*），
	 *
	 * 使用方式（Blueprint）：
	 * 1. 传入 JsonString
	 * 2. 选择目标 Struct 类型（UScriptStruct）
	 * 3. 通过 Success / Failed 委托获取结果：InstancedStruct
	 * 4. 通过 GetInstancedStructValue 获取 Struct 实例
	 *
	 * @param WorldContextObject
	 *   世界上下文对象
	 *   - 用于 RegisterWithGameInstance 和 日志打印
	 *
	 * @param JsonString
	 *   输入的 Json 字符串
	 *   - 必须是合法 Json
	 *   - 必须与目标 Struct 结构匹配
	 *
	 * @param StructType
	 *   目标 Struct 类型（UScriptStruct）
	 *   - 必须显式指定类型
	 *
	 * @return
	 *   返回异步任务节点实例
	 *   - 通过 Success / Failed 委托获取结果
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Async_JsonToStruct", Category="FH|UnrealJson|Convert|Async", 
		meta=(BlueprintInternalUseOnly="true", DefaultToSelf="WorldContextObject", AdvancedDisplay="WorldContextObject"))
	static UAsync_JsonStringToStruct* JsonStringToStructAsync(UObject* WorldContextObject, const FString& JsonString, UScriptStruct* StructType);

protected:
	virtual void Activate() override;
	FString GetCallerName() const;
	
public:
	UFUNCTION(BlueprintCallable, Category="FH|UnrealJson|Convert|Async")
	void EndTask();
};
