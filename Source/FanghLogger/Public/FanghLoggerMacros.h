#pragma once

#include "FH_Logger.h"

#if !UE_BUILD_SHIPPING

// ===== 默认：使用 __FUNCTION__ =====
#define FH_LOG(Category, Level, Context, Content) \
FH_Logger::Log( \
	Category, \
	Level, \
	Context, \
	Content, \
	ANSI_TO_TCHAR(__FUNCTION__) \
);

// ===== 高级：显式指定 FunctionName（Async / Lambda 用）=====
// 传参顺序：Category, Level, Context, FunctionNam, Content 
#define FH_LOG_F(Category, Level, Context, FunctionName, Content) \
FH_Logger::Log( \
	Category, \
	Level, \
	Context, \
	Content, \
	FunctionName \
);

#else
#define FH_LOG(Category, Level, Context, Content);
#define FH_LOG_F(Category, Level, Context, FunctionName, Content);
#endif

// ===== Log Categories（跨模块导出）=====
// 需要在 FanghLoggerMacros.cpp 中实现 DEFINE_LOG_CATEGORY(LogCategory) 宏
FANGHLOGGER_API DECLARE_LOG_CATEGORY_EXTERN(LogUnrealJsonConvert, Log, All);
FANGHLOGGER_API DECLARE_LOG_CATEGORY_EXTERN(LogUnrealJsonMake, Log, All);
FANGHLOGGER_API DECLARE_LOG_CATEGORY_EXTERN(LogUnrealJsonRead, Log, All);