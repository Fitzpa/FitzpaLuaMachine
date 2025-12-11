# LuaMachine Copilot Instructions

## Project Overview

LuaMachine is an Unreal Engine 5 plugin that adds Lua scripting capabilities to Unreal Engine projects. Unlike other Lua plugins, LuaMachine doesn't automatically expose the entire Unreal Engine API. Instead, it gives developers full control over which features to expose to Lua scripts via Blueprints or C++.

**Key Features:**
- Lua 5.3 integration with Unreal Engine 5
- Blueprint and C++ API for exposing functionality to Lua
- Multiple isolated Lua virtual machines (LuaState)
- UMG widget integration with syntax highlighting
- Cross-platform support: Windows, Mac, Linux (x86_64 & AArch64), Android, iOS
- Bytecode compilation support
- Reflection API for advanced Unreal Engine integration

## Technology Stack

- **Engine:** Unreal Engine 5 (UE4 support in separate branch)
- **Languages:** C++, Lua 5.3, Blueprints
- **Build System:** Unreal Build Tool (.Build.cs files)
- **Third-Party Libraries:** Lua 5.3 (static libraries in Source/ThirdParty/)

## Repository Structure

```
/
├── Source/
│   ├── LuaMachine/          # Main runtime module
│   │   ├── Public/          # Public API headers
│   │   ├── Private/         # Implementation files
│   │   └── LuaMachine.Build.cs
│   ├── LuaMachineEditor/    # Editor-only module
│   └── ThirdParty/          # Lua static libraries by platform
├── Content/                 # Example Lua scripts and test assets
│   ├── Scripts/             # Example Lua scripts
│   └── Tests/               # Functional test blueprints
├── Docs/                    # API documentation
├── Tutorials/               # Tutorial markdown files
├── Config/                  # Plugin configuration
├── Resources/               # Plugin resources (icons, etc.)
└── LuaMachine.uplugin      # Plugin descriptor
```

## Core Components

### Key Classes (Source/LuaMachine/Public/)

1. **LuaState.h** - Represents a Lua virtual machine (singleton per class)
2. **LuaValue.h** - Bridge between Unreal and Lua values (supports all Lua types)
3. **LuaBlueprintFunctionLibrary.h** - Blueprint nodes for Lua operations
4. **LuaComponent.h** - ActorComponent that maps to Lua tables
5. **LuaCode.h** - Asset type for Lua scripts
6. **LuaUserDataObject.h** - Base class for creating custom Lua object types
7. **LuaBlueprintPackage.h** - Groups functions to expose to multiple LuaStates
8. **LuaMultiLineEditableTextBox.h** - UMG widget with Lua syntax highlighting

### Module Dependencies

**LuaMachine (Runtime):**
- Public: Core, HTTP, Json, PakFile
- Private: CoreUObject, Engine, Slate, SlateCore, UMG, InputCore
- Editor-only: UnrealEd, Projects

## Coding Standards & Conventions

### C++ Conventions

1. **Headers:**
   - All public headers in `Source/LuaMachine/Public/`
   - Use include guards and UCLASS/USTRUCT macros appropriately
   - Copyright notice: `// Copyright 2018-2023 - Roberto De Ioris`

2. **Naming:**
   - Classes: ULuaState, ULuaComponent, etc. (U prefix for UObject-derived)
   - Structs: FLuaValue (F prefix)
   - Member variables: Standard Unreal naming (no hungarian notation required)

3. **UFunction Requirements:**
   - Functions exposed to Lua must use FLuaValue for parameters and return values
   - Use `UFUNCTION()` macro for Blueprint/Lua callable functions
   - Use `GET_FUNCTION_NAME_CHECKED` macro for type-safe function name references

4. **Module Structure:**
   - Runtime code in LuaMachine module
   - Editor-only code in LuaMachineEditor module
   - Platform-specific library linking in .Build.cs files

### Lua Conventions

1. **Scripts:**
   - Example scripts in `Content/Scripts/`
   - Use Lua 5.3 syntax
   - `print()` automatically maps to Unreal Output Log

2. **Integration:**
   - Access Unreal functions through global table
   - Return values from scripts as needed
   - Use `require()` for modular script organization

## Building

### Prerequisites
- Unreal Engine 5 installed
- C++ compiler for your platform (MSVC, Clang, or GCC)
- Lua 5.3 static libraries (already included in Source/ThirdParty/)

### Build Process
This is an Unreal Engine plugin - it builds automatically when:
1. Placed in a project's `Plugins/` directory
2. The project is opened or compiled
3. The Unreal Editor regenerates project files

### Building Lua Libraries
If you need to rebuild Lua static libraries for any platform, see `BuildingNotes.md` for detailed platform-specific instructions.

## Testing

### Functional Tests
- Located in `Content/Tests/` as Blueprint assets
- Run via Unreal Editor's Automation Tool (Session Frontend)
- Enable plugin content visibility in Content Browser to access tests
- Tests cover: file execution, global calls, table operations, component lifecycle

### Manual Testing
1. Enable the LuaMachine plugin in your project
2. Create a LuaState Blueprint
3. Add Lua scripts to Content directory
4. Use LuaBlueprintFunctionLibrary nodes in Level Blueprint
5. Check Output Log for print() statements

## Platform Support

### Supported Platforms
- **Windows:** 64-bit (x64)
- **Mac:** Universal binary (x86_64 + ARM64)
- **Linux:** x86_64 and AArch64
- **Android:** ARMv7 and ARM64 (API level 24+)
- **iOS:** ARM64

### Platform-Specific Notes
- **Android:** Minimum API level 24 required
- **iOS:** `os_execute` disabled for security
- **All platforms:** Bytecode is converted to 32-bit size_t at runtime for compatibility

## Key Documentation Files

- **README.md** - Quick start guide and overview
- **Docs/LuaBlueprintFunctionLibrary.md** - Complete Blueprint API reference
- **Docs/LuaComponent.md** - LuaComponent usage guide
- **Docs/ReflectionAPI.md** - Advanced reflection API documentation
- **Docs/TipsAndTricks.md** - C++ integration tips
- **Docs/LuaCoroutines.md** - Lua thread/coroutine support
- **Tutorials/** - Step-by-step integration tutorials

## Development Workflow

### Making Changes

1. **C++ Changes:**
   - Modify headers in `Public/` or implementations in `Private/`
   - Update `.Build.cs` if adding new dependencies
   - Rebuild plugin through Unreal Editor

2. **Blueprint/Asset Changes:**
   - Add test assets in `Content/Tests/`
   - Keep example scripts in `Content/Scripts/`

3. **Documentation:**
   - Update relevant .md files when changing API
   - Add tutorials for new major features

### Best Practices

1. **LuaState Management:**
   - Each LuaState class is a singleton (one instance per process)
   - Use multiple LuaStates to isolate different domains (e.g., config, logic, mods)
   - LuaStates are lazy-loaded on first use

2. **Memory Management:**
   - Tables are passed by reference between Lua and Unreal
   - Use the Lua and Unreal garbage collectors appropriately
   - Monitor memory in the LuaMachine Debugger (Window/Developer Tools)

3. **Error Handling:**
   - Enable LogError property on LuaState for debugging
   - Implement LuaError event for custom error handling
   - Use LuaConsole in editor for testing

4. **Performance:**
   - Bytecode compilation recommended for production
   - Use LuaCode assets for built-in scripts
   - Filesystem scripts allow post-package modification

## Security Considerations

1. **Script Source Control:**
   - Sign pak files to prevent unauthorized script modifications
   - Use bytecode compilation to obscure logic
   - Consider which Lua standard libraries to enable

2. **API Exposure:**
   - Only expose necessary functionality to Lua
   - Validate all inputs from Lua scripts
   - Use LuaUserDataObject for controlled object exposure

## Support & Resources

- **GitHub:** https://github.com/rdeioris/LuaMachine
- **Discord:** https://discord.gg/eTXDfEU
- **Marketplace:** https://www.unrealengine.com/marketplace/luamachine
- **Commercial Support:** contact info at blitter.studio

## Version Information

- **Current Version:** 20250604
- **Unreal Engine:** 5.x (UE4 support in 'ue4' branch)
- **Lua Version:** 5.3
- **Experimental:** LuaU support available in 'luau' branch (Windows only)
