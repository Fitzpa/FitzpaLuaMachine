// Copyright 2018-2023 - Roberto De Ioris

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "LuaState.h"
#include "LuaValue.h"
#include "LuaCommonUIWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLuaCommonUIWidgetActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLuaCommonUIWidgetDeactivated);

/**
 * Base class for Common UI widgets with Lua scripting support
 * Allows Lua scripts to control and respond to Common UI widget lifecycle events
 */
UCLASS(Blueprintable, ClassGroup=(UI))
class /**
 * Construct the widget with the given object initializer.
 * @param ObjectInitializer Initialization parameters provided by the engine.
 */
 
/**
 * Invoke a function stored in this widget's Lua table with the provided arguments.
 * @param Name The name of the Lua function to call.
 * @param Args Positional arguments to pass to the Lua function.
 * @returns The value returned by the Lua function.
 */

/**
 * Retrieve a field value from this widget's Lua table.
 * @param Name The field name to read from the Lua table.
 * @returns The value of the requested field; returns an empty/invalid FLuaValue if the field does not exist.
 */

/**
 * Set or create a field in this widget's Lua table.
 * @param Name The field name to set in the Lua table.
 * @param Value The value to store at the specified field.
 */

/**
 * Obtain the ULuaState instance associated with this widget.
 * @returns Pointer to the widget's ULuaState, or nullptr if no state is available.
 */

/**
 * Access the widget's Lua table representation.
 * @returns The FLuaValue that represents this widget's Lua table.
 */

/**
 * Called when the widget is constructed; performs widget initialization and Lua table setup.
 */

/**
 * Called when the widget is destructed; performs cleanup related to the widget and its Lua integration.
 */

/**
 * Called when the widget is activated; triggers activation-related Lua behaviour and events.
 */

/**
 * Called when the widget is deactivated; triggers deactivation-related Lua behaviour and events.
 */

/**
 * Create and populate the Lua table that represents this widget, linking C++ state and exposed fields/methods.
 */

/**
 * If a Lua function with the given name exists on the widget's Lua table, call it.
 * @param FunctionName The name of the Lua function to call.
 * @returns `true` if the function existed and was invoked, `false` otherwise.
 */
LUAMACHINE_API ULuaCommonUIWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	ULuaCommonUIWidget(const FObjectInitializer& ObjectInitializer);

	// Lua State to use for this widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	TSubclassOf<ULuaState> LuaState;

	// Lua table that represents this widget's data and methods
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	TMap<FString, FLuaValue> Table;

	// Optional Lua function to call when widget is activated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	FString OnActivatedLuaFunction;

	// Optional Lua function to call when widget is deactivated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	FString OnDeactivatedLuaFunction;

	// Optional Lua function to call when widget is constructed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	FString OnConstructedLuaFunction;

	// Optional Lua function to call when widget is destructed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	FString OnDestructedLuaFunction;

	// Enable logging of Lua errors
	UPROPERTY(EditAnywhere, Category = "Lua")
	bool bLogError;

	// Blueprint-assignable events
	UPROPERTY(BlueprintAssignable, Category = "Lua")
	FLuaCommonUIWidgetActivated OnLuaActivated;

	UPROPERTY(BlueprintAssignable, Category = "Lua")
	FLuaCommonUIWidgetDeactivated OnLuaDeactivated;

	// Call a Lua function from the widget's table
	UFUNCTION(BlueprintCallable, Category = "Lua", meta = (AutoCreateRefTerm = "Args"))
	FLuaValue LuaCallFunction(const FString& Name, TArray<FLuaValue> Args);

	// Get a field from the widget's Lua table
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lua")
	FLuaValue LuaGetField(const FString& Name);

	// Set a field in the widget's Lua table
	UFUNCTION(BlueprintCallable, Category = "Lua")
	void LuaSetField(const FString& Name, FLuaValue Value);

	// Get the internal Lua state instance
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lua")
	ULuaState* LuaWidgetGetState();

	// Get the Widget's Lua table
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lua")
	FLuaValue GetWidgetLuaTable() const { return WidgetLuaTable; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	// Initialize the Lua table for this widget
	void InitializeLuaTable();

	// Call a Lua function if it exists
	bool CallLuaFunctionIfExists(const FString& FunctionName);

	// The Lua table value representing this widget
	FLuaValue WidgetLuaTable;
};