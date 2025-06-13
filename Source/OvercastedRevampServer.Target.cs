using UnrealBuildTool;
using System.Collections.Generic;

public class OvercastedRevampServerTarget : TargetRules
{
    public OvercastedRevampServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
        bUseIris = true;
        BuildEnvironment = TargetBuildEnvironment.Unique;
        bUseLoggingInShipping = true;
        ExtraModuleNames.Add("OvercastedRevamp");
    }
}