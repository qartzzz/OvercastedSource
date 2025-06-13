// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class OvercastedRevampTarget : TargetRules
{
	public OvercastedRevampTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		bUseIris = true;
		ExtraModuleNames.Add("OvercastedRevamp");
	}
}
