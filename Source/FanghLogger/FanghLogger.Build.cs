using UnrealBuildTool;

public class FanghLogger : ModuleRules
{
    public FanghLogger(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core"
            }
        );
        
        // 确保 Public 文件夹路径正确
        PublicIncludePaths.Add(ModuleDirectory + "/Public");

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}