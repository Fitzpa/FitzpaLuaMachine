// Copyright 2018-2023 - Roberto De Ioris

#include "LuaCommonUIWidget.h"
#include "LuaBlueprintFunctionLibrary.h"

/**
 * @brief Initializes a ULuaCommonUIWidget instance and sets default behavior.
 *
 * Sets the widget's error-logging flag (bLogError) to true.
 */
ULuaCommonUIWidget::ULuaCommonUIWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bLogError = true;
}

/**
 * @brief Sets up the widget's Lua environment and invokes its Lua construction hook.
 *
 * Initializes the per-widget Lua table and, if a global or table function is configured
 * in OnConstructedLuaFunction, calls that Lua function with the widget table as the first argument.
 */
void ULuaCommonUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeLuaTable();
	CallLuaFunctionIfExists(OnConstructedLuaFunction);
}

/**
 * @brief Performs widget teardown and invokes the Lua destruction hook if present.
 *
 * Calls the configured Lua `OnDestructedLuaFunction` (when available) to allow Lua-side cleanup, then forwards teardown to the superclass implementation.
 */
void ULuaCommonUIWidget::NativeDestruct()
{
	CallLuaFunctionIfExists(OnDestructedLuaFunction);
	
	Super::NativeDestruct();
}

/**
 * @brief Handles widget activation by invoking Lua hook and notifying listeners.
 *
 * Calls the configured Lua "OnActivated" function for this widget if it exists, then broadcasts the OnLuaActivated delegate.
 */
void ULuaCommonUIWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	CallLuaFunctionIfExists(OnActivatedLuaFunction);
	OnLuaActivated.Broadcast();
}

/**
 * @brief Handle widget deactivation by invoking the Lua hook, broadcasting deactivation, then delegating to the base implementation.
 *
 * Calls the Lua function specified by OnDeactivatedLuaFunction if it exists, broadcasts the OnLuaDeactivated multicast delegate, and then calls the superclass NativeOnDeactivated.
 */
void ULuaCommonUIWidget::NativeOnDeactivated()
{
	CallLuaFunctionIfExists(OnDeactivatedLuaFunction);
	OnLuaDeactivated.Broadcast();

	Super::NativeOnDeactivated();
}

/**
 * @brief Initializes the widget's Lua table and populates it with the widget reference and custom fields.
 *
 * Creates a per-widget Lua table, sets a "Widget" field containing a Lua-referenced object for this widget,
 * and copies all entries from the widget's Table property into that Lua table. If the configured LuaState
 * or its runtime instance cannot be obtained, the function logs an error when bLogError is true and returns
 * without modifying WidgetLuaTable.
 */
void ULuaCommonUIWidget::InitializeLuaTable()
{
	if (!LuaState)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaCommonUIWidget: LuaState is not set"));
		}
		return;
	}

	ULuaState* State = ULuaBlueprintFunctionLibrary::LuaGetState(this, LuaState);
	if (!State)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaCommonUIWidget: Failed to get LuaState instance"));
		}
		return;
	}

	// Create a Lua table for this widget
	WidgetLuaTable = ULuaBlueprintFunctionLibrary::LuaCreateTable(this, LuaState);

	// Add the widget reference to the table
	ULuaBlueprintFunctionLibrary::LuaTableSetField(WidgetLuaTable, TEXT("Widget"), ULuaBlueprintFunctionLibrary::LuaCreateObject(this));

	// Add all custom fields from the Table property
	for (const TPair<FString, FLuaValue>& Pair : Table)
	{
		ULuaBlueprintFunctionLibrary::LuaTableSetField(WidgetLuaTable, Pair.Key, Pair.Value);
	}
}

/**
 * Attempts to invoke a global Lua function by name, supplying this widget's Lua table as the first argument.
 *
 * If the widget's Lua state is not set or the named global is not a callable function, the call is not made.
 * If `bLogError` is true, missing Lua state or a non-callable/missing function will produce log output.
 *
 * @param FunctionName Name of the global Lua function to call.
 * @return true if a callable global function was found and invoked, false otherwise.
 */
bool ULuaCommonUIWidget::CallLuaFunctionIfExists(const FName& FunctionName)
{
	if (FunctionName.IsNone())
	{
		return false;
	}

	if (!LuaState)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaCommonUIWidget: LuaState is not set"));
		}
		return false;
	}

	FString FunctionNameString = FunctionName.ToString();

	// Try to get the function from the global table
	FLuaValue FunctionValue = ULuaBlueprintFunctionLibrary::LuaGetGlobal(this, LuaState, FunctionNameString);
	
	if (ULuaBlueprintFunctionLibrary::LuaValueIsFunction(FunctionValue))
	{
		// Call the function with the widget table as the first argument
		TArray<FLuaValue> Args;
		Args.Add(WidgetLuaTable);
		
		FLuaValue Result = ULuaBlueprintFunctionLibrary::LuaValueCall(FunctionValue, Args);
		
		return true;
	}
	else if (bLogError)
	{
		UE_LOG(LogLuaMachine, Verbose, TEXT("LuaCommonUIWidget: Lua function '%s' not found or not callable"), *FunctionNameString);
	}

	return false;
}

/**
 * @brief Calls a function stored in this widget's Lua table, passing the widget table as `self`.
 *
 * Retrieves the field `Name` from the widget's Lua table and, if it is a function, calls it with
 * the widget table inserted as the first argument followed by `Args`. Logs an error or warning when
 * the widget table is uninitialized or the named field is not a callable function.
 *
 * @param Name The field name in the widget's Lua table to call.
 * @param Args Positional arguments to pass to the Lua function (the widget table is prepended as `self` automatically).
 * @return FLuaValue The value returned by the Lua function, or `nil` if the widget table is uninitialized or the function is not found/callable.
 */
FLuaValue ULuaCommonUIWidget::LuaCallFunction(const FString& Name, TArray<FLuaValue> Args)
{
	if (WidgetLuaTable.Type != ELuaValueType::Table)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaCommonUIWidget: Widget Lua table is not initialized"));
		}
		return ULuaBlueprintFunctionLibrary::LuaCreateNil();
	}

	FLuaValue FunctionValue = ULuaBlueprintFunctionLibrary::LuaTableGetField(WidgetLuaTable, Name);
	
	if (ULuaBlueprintFunctionLibrary::LuaValueIsFunction(FunctionValue))
	{
		// Insert the widget table as the first argument (self)
		Args.Insert(WidgetLuaTable, 0);
		return ULuaBlueprintFunctionLibrary::LuaValueCall(FunctionValue, Args);
	}
	else if (bLogError)
	{
		UE_LOG(LogLuaMachine, Warning, TEXT("LuaCommonUIWidget: Function '%s' not found in widget table"), *Name);
	}

	return ULuaBlueprintFunctionLibrary::LuaCreateNil();
}

/**
 * @brief Retrieve a field value from the widget's Lua table.
 *
 * Logs an error if the widget Lua table is not initialized.
 *
 * @param Name Name of the field to retrieve from the widget table.
 * @return FLuaValue The field's value, or `nil` if the widget table is not initialized or the field does not exist.
 */
FLuaValue ULuaCommonUIWidget::LuaGetField(const FString& Name)
{
	if (WidgetLuaTable.Type != ELuaValueType::Table)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaCommonUIWidget: Widget Lua table is not initialized"));
		}
		return ULuaBlueprintFunctionLibrary::LuaCreateNil();
	}

	return ULuaBlueprintFunctionLibrary::LuaTableGetField(WidgetLuaTable, Name);
}

/**
 * @brief Sets a field in the widget's Lua table.
 *
 * Assigns the given Value to the field named Name inside this widget's Lua
 * table. If the widget Lua table is not initialized, the function logs an
 * error when bLogError is true and returns without modifying any table.
 *
 * @param Name Name of the field to set in the widget Lua table.
 * @param Value Value to assign to the field.
 */
void ULuaCommonUIWidget::LuaSetField(const FString& Name, FLuaValue Value)
{
	if (WidgetLuaTable.Type != ELuaValueType::Table)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaCommonUIWidget: Widget Lua table is not initialized"));
		}
		return;
	}

	ULuaBlueprintFunctionLibrary::LuaTableSetField(WidgetLuaTable, Name, Value);
}

/**
 * @brief Get the Lua state associated with this widget.
 *
 * @return ULuaState* Pointer to the widget's Lua state, or nullptr if the widget has no Lua state.
 */
ULuaState* ULuaCommonUIWidget::LuaWidgetGetState()
{
	if (!LuaState)
	{
		return nullptr;
	}

	return ULuaBlueprintFunctionLibrary::LuaGetState(this, LuaState);
}