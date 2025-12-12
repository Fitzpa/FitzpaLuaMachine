// Copyright 2018-2023 - Roberto De Ioris

using UnrealBuildTool;

public class LuaMachine : ModuleRules
{
    /// <summary>
    /// Configures build settings for the LuaMachine module, declaring include paths, public and private dependencies, editor-only dependencies, and selecting the platform-specific third-party Lua libraries to link.
    /// </summary>
    /// <param name="Target">The build target rules used to configure module settings for the current target platform and build configuration.</param>
    public LuaMachine(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "HTTP",
                "Json",
                "PakFile"
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UMG",
                "InputCore",
                "CommonUI",
                "ModelViewViewModel",
				// ... add private dependencies that you statically link with here ...	
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]{
                "UnrealEd",
                "Projects"
            });
        }

        string ThirdPartyDirectory = System.IO.Path.Combine(ModuleDirectory, "..", "ThirdParty");

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyDirectory, "x64", "liblua53_win64.lib"));
        }

        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyDirectory, "x64", "liblua53_mac.a"));
        }

        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyDirectory, "x64", "liblua53_linux64.a"));
        }

        else if (Target.Platform == UnrealTargetPlatform.LinuxArm64)
        {
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyDirectory, "ARM64", "liblua53_linux_aarch64.a"));
        }

        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyDirectory, "ARMv7", "liblua53_android.a"));
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyDirectory, "ARM64", "liblua53_android64.a"));
        }

        else if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyDirectory, "ARM64", "liblua53_ios.a"));
        }

    }
}