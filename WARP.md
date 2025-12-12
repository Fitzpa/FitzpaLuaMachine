# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

# Project: FitzpaLuaMachine (LuaMachine)

This repository contains the **LuaMachine** plugin for Unreal Engine, which provides a bridge between Unreal Engine (Blueprints/C++) and Lua scripting.

**Note**: The `master` branch targets **Unreal Engine 5**. Use the `ue4` branch for Unreal Engine 4.

## Core Architecture

The plugin is divided into two main modules and a third-party dependency:

1.  **LuaMachine (Runtime)**: `Source/LuaMachine/`
    *   Contains the core logic for the Lua bridge.
    *   **Key Classes**:
        *   `ULuaState` (`LuaState.h`): Represents a Lua Virtual Machine. Handles startup, script loading, and the global table.
        *   `FLuaValue` (`LuaValue.h`): A variant struct that bridges types between UE and Lua (Nil, Bool, Int, Number, String, Function, Table, UObject, etc.).
        *   `ULuaComponent` (`LuaComponent.h`): ActorComponent that runs Lua scripts and maps a Lua table to an Actor.
        *   `ULuaBlueprintFunctionLibrary`: Exposes LuaMachine API to Blueprints.
        *   `ULuaUserDataObject`: Base class for creating custom Lua userdata types in C++.

2.  **LuaMachineEditor (Editor)**: `Source/LuaMachineEditor/`
    *   Editor-specific functionality, tools, and UI.
    *   Includes the **LuaMachine Debugger** and **Lua Console** integration.

3.  **ThirdParty**: `Source/ThirdParty/`
    *   Contains Lua 5.3 headers (`lua/`) and precompiled static libraries for supported platforms (`x64`, `ARM64`, etc.).

## Development & Build

### Building the Plugin
*   This is a standard Unreal Engine plugin. It is built via the **Unreal Build Tool (UBT)** as part of an Unreal Engine project.
*   **Compilation**:
    *   Ensure C++ is enabled in the host project.
    *   Regenerate project files if source files are added/removed.
    *   Build the host project to compile the plugin.

### Building Lua Static Libraries
If you need to rebuild the underlying Lua static libraries (e.g., for a new platform or custom Lua version):
*   Refer to `BuildingNotes.md` for specific command-line instructions (`make`, `cl`, `clang`, etc.) for Windows, Mac, Linux, Android, and iOS.
*   Headers are located in `Source/ThirdParty/lua/`.

## Coding Guidelines

*   **Lua <-> UE Bridge**:
    *   Use `FLuaValue` to pass data between C++ and Lua.
    *   Use `ULuaBlueprintFunctionLibrary` for static helper functions.
    *   `ULuaState` is the entry point for executing scripts and managing the VM.
*   **Lua Scripts**:
    *   Scripts are typically located in the `Content/` directory.
    *   The plugin supports both filesystem-based scripts (for iteration) and `LuaCode` assets (for packaging).

## Testing

*   **Functional Tests**: The project includes functional tests implemented as Blueprints.
    *   Enable plugin content visualization in the Content Browser.
    *   Use the **Session Frontend / Automation** tool in Unreal Editor to execute Project-related tests.
