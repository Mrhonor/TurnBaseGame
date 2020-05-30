// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TurnBaseGame : ModuleRules
{
	public TurnBaseGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "UMG", "GameplayAbilities", "GameplayTags", "GameplayTasks" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ProceduralMeshComponent", "Slate", "SlateCore" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        PublicIncludePaths.AddRange(
        new string[] {
        "TurnBaseGame/Public/Character",
        "TurnBaseGame/Public/Character/Player",
        "TurnBaseGame/Public/Scene"
        }
        );

        PrivateIncludePaths.AddRange(
        new string[] {
        "TurnBaseGame/Private/Character",
        "TurnBaseGame/Private/Character/Player",
        "TurnBaseGame/Private/Scene"
        }
        );
    }
}
