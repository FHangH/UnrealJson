#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonConvertTypes.h"
#include "FuncLib_Converter.generated.h"

struct FInstancedStruct;

/**
 * Json ↔ Struct 转换函数库
 *
 * 设计目标：
 * 1. 支持「任意 Blueprint Struct（Wildcard）」的 Json 转换
 * 2. 不依赖模板（Blueprint 不支持）
 * 3. 通过 CustomThunk + Stack 反射获取真实 Struct 类型与内存
 * 4. 结果通过 Enum + ErrorMessage 明确回传，便于蓝图分支处理
 *
 * 注意：
 * - 这里的 int32 / int32& 只是 Wildcard 占位符
 * - 实际传入的是 Blueprint Struct
 * - 类型信息在 execXXX 中通过 FStructProperty 动态解析
 */
UCLASS()
class STRUCTJSONCONVERTER_API UFuncLib_Converter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Struct → JsonString
	 *
	 * Blueprint 行为：
	 * - Struct 参数表现为「通配符 Struct」
	 * - Result 会被展开为 Exec 分支（Success / Failed）
	 * - OutJsonString 与 ErrorMessage 为普通数据输出
	 *
	 * 关键点：
	 * - 使用 CustomThunk 捕获任意 Struct
	 * - 通过 FJsonObjectConverter::UStructToJsonObjectString
	 *   直接序列化 Struct 内存
	 *
	 * @param Struct
	 *   Blueprint 中的任意 Struct（Wildcard）
	 *   ⚠️ 在 C++ 中以 const int32& 作为占位声明
	 *
	 * @param OutJsonString
	 *   转换成功时输出的 Json 字符串
	 *
	 * @param Result
	 *   转换结果枚举
	 *   - Success：转换成功
	 *   - Failed ：转换失败
	 *   ⚠️ 通过 ExpandEnumAsExecs 展开为 Exec 分支
	 *
	 * @param ErrorMessage
	 *   失败时的错误信息
	 *   成功时通常为空字符串
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, DisplayName="StructToJsonString", Category="FH|UnrealJson|Convert", 
		meta=(CustomStructureParam="Struct", ExpandEnumAsExecs="Result"))
	static void BP_StructToJsonString(const int32& Struct, FString& OutJsonString, EJsonConvertResult& Result, FString& ErrorMessage);
	
	/**
	 * CustomThunk 实现
	 *
	 * 负责：
	 * - 从 VM Stack 中解析真实 Struct 类型（UScriptStruct）
	 * - 获取 Struct 实际内存地址
	 * - 调用 JsonObjectConverter 进行序列化
	 */
	DECLARE_FUNCTION(execBP_StructToJsonString);
	
	/**
	 * JsonString → Struct
	 *
	 * Blueprint 行为：
	 * - Struct 为通配符输出 Struct
	 * - JsonString 为输入 Json 字符串
	 * - Result 被展开为 Exec 分支
	 *
	 * 实现策略（重要）：
	 * - UE 不支持「Wildcard JsonString → UStruct」一步完成
	 * - 必须先：
	 *   JsonString → FJsonObject
	 * - 再：
	 *   FJsonObject → Struct
	 *
	 * 这样才能支持任意 Blueprint Struct
	 *
	 * @param JsonString
	 *   输入 Json 字符串
	 *
	 * @param Struct
	 *   输出 Struct（Wildcard）
	 *   ⚠️ C++ 中以 int32& 占位，实际由 VM 传入 Struct 内存
	 *
	 * @param Result
	 *   转换结果枚举（展开为 Exec 分支）
	 *
	 * @param ErrorMessage
	 *   失败原因描述
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, DisplayName="JsonStringToStruct", Category="FH|UnrealJson|Convert", 
		meta=(CustomStructureParam="Struct", ExpandEnumAsExecs="Result"))
	static void BP_JsonStringToStruct(const FString& JsonString, int32& Struct, EJsonConvertResult& Result, FString& ErrorMessage);

	/**
	 * CustomThunk 实现
	 *
	 * 核心流程：
	 * 1. 从 Stack 中获取 StructProperty 与 StructData
	 * 2. JsonString → FJsonObject
	 * 3. FJsonObject → Struct 内存
	 *
	 * 所有失败路径都会：
	 * - 设置 Result = Failed
	 * - 填充 ErrorMessage
	 */
	DECLARE_FUNCTION(execBP_JsonStringToStruct);
};
