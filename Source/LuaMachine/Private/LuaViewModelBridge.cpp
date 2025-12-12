// Copyright 2018-2023 - Roberto De Ioris

#include "LuaViewModelBridge.h"
#include "LuaBlueprintFunctionLibrary.h"

/**
 * @brief Constructs a ULuaViewModelBridge and enables error logging by default.
 *
 * Initializes the bridge instance with error logging turned on (`bLogError = true`).
 */
ULuaViewModelBridge::ULuaViewModelBridge()
{
	bLogError = true;
}

/**
 * @brief Initializes the Lua table that represents this view model and populates it with configured fields.
 *
 * Validates that a LuaState is set and that a ULuaState instance can be obtained; if validation fails the method logs an error (when logging is enabled) and returns without modifying state.
 * On success, creates the ViewModel Lua table, stores a reference to this view model on the table, and copies all key/value pairs from the bridge's Table property into the Lua table.
 */
void ULuaViewModelBridge::InitializeLuaViewModel()
{
	if (!LuaState)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaViewModelBridge: LuaState is not set"));
		}
		return;
	}

	ULuaState* State = ULuaBlueprintFunctionLibrary::LuaGetState(this, LuaState);
	if (!State)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaViewModelBridge: Failed to get LuaState instance"));
		}
		return;
	}

	// Create a Lua table for this ViewModel
	ViewModelLuaTable = ULuaBlueprintFunctionLibrary::LuaCreateTable(this, LuaState);

	// Add the ViewModel reference to the table
	ULuaBlueprintFunctionLibrary::LuaTableSetField(ViewModelLuaTable, TEXT("ViewModel"), ULuaBlueprintFunctionLibrary::LuaCreateObject(this));

	// Add all custom fields from the Table property
	for (const TPair<FString, FLuaValue>& Pair : Table)
	{
		ULuaBlueprintFunctionLibrary::LuaTableSetField(ViewModelLuaTable, Pair.Key, Pair.Value);
	}
}

/**
 * @brief Retrieve a property's value from the ViewModel's Lua table, allowing an optional Lua getter to override the lookup.
 *
 * If an OnGetPropertyLuaFunction is defined and returns a non-nil value for the given property, that value is used.
 * Otherwise the value is read directly from the ViewModel Lua table. If the ViewModel table is not initialized, this returns nil.
 *
 * @param PropertyName Name of the property to retrieve.
 * @return FLuaValue The Lua value of the requested property, or nil if the ViewModel table is uninitialized or the property is absent.
 */
FLuaValue ULuaViewModelBridge::LuaGetProperty(const FString& PropertyName)
{
	if (ViewModelLuaTable.Type != ELuaValueType::Table)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaViewModelBridge: ViewModel Lua table is not initialized. Call InitializeLuaViewModel first."));
		}
		return ULuaBlueprintFunctionLibrary::LuaCreateNil();
	}

	// Try to call the OnGetPropertyLuaFunction if it exists
	if (!OnGetPropertyLuaFunction.IsEmpty())
	{
		TArray<FLuaValue> Args;
		Args.Add(ViewModelLuaTable);
		Args.Add(ULuaBlueprintFunctionLibrary::LuaCreateString(PropertyName));
		
		FLuaValue Result;
		if (CallLuaFunctionIfExists(OnGetPropertyLuaFunction, Args, Result))
		{
			// If the result is not nil, use it; otherwise fall through to default behavior
			if (!ULuaBlueprintFunctionLibrary::LuaValueIsNil(Result))
			{
				return Result;
			}
		}
	}

	// Default: get from table
	return ULuaBlueprintFunctionLibrary::LuaTableGetField(ViewModelLuaTable, PropertyName);
}

/**
 * @brief Sets a property on the ViewModel Lua table, optionally delegating to a custom Lua setter.
 *
 * Attempts to update the named property on the ViewModel Lua table. If an OnSetPropertyLuaFunction is defined, that function is invoked with (ViewModelTable, PropertyName, Value); if it returns the boolean `true` the change is considered handled, if it returns `false` the change is rejected, and if it returns `nil` or a non-boolean the bridge falls back to setting the field on the ViewModel table. If no custom setter is defined the field is set directly. When the property is actually set, a field-value-changed broadcast is emitted. If the ViewModel table is not initialized, the function returns immediately (and may log an error).
 *
 * @param PropertyName Name of the property/field to set on the ViewModel Lua table.
 * @param Value The Lua value to assign to the property.
 */
void ULuaViewModelBridge::LuaSetProperty(const FString& PropertyName, FLuaValue Value)
{
	if (ViewModelLuaTable.Type != ELuaValueType::Table)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaViewModelBridge: ViewModel Lua table is not initialized. Call InitializeLuaViewModel first."));
		}
		return;
	}

	bool bPropertyWasSet = false;

	// Try to call the OnSetPropertyLuaFunction if it exists
	if (!OnSetPropertyLuaFunction.IsEmpty())
	{
		TArray<FLuaValue> Args;
		Args.Add(ViewModelLuaTable);
		Args.Add(ULuaBlueprintFunctionLibrary::LuaCreateString(PropertyName));
		Args.Add(Value);
		
		FLuaValue Result;
		if (CallLuaFunctionIfExists(OnSetPropertyLuaFunction, Args, Result))
		{
			// The Lua function should return true if it handled the property,
			// false if it rejected the change, or nil to use default behavior
			if (ULuaBlueprintFunctionLibrary::LuaValueIsBoolean(Result))
			{
				bool bHandled = ULuaBlueprintFunctionLibrary::Conv_LuaValueToBool(Result);
				if (bHandled)
				{
					// Function handled the property successfully
					bPropertyWasSet = true;
				}
				// else: Function explicitly rejected the change (returned false)
			}
			else
			{
				// Function returned nil or non-bool, use default behavior
				ULuaBlueprintFunctionLibrary::LuaTableSetField(ViewModelLuaTable, PropertyName, Value);
				bPropertyWasSet = true;
			}
		}
		else
		{
			// Function doesn't exist, use default behavior
			ULuaBlueprintFunctionLibrary::LuaTableSetField(ViewModelLuaTable, PropertyName, Value);
			bPropertyWasSet = true;
		}
	}
	else
	{
		// Default: set in table
		ULuaBlueprintFunctionLibrary::LuaTableSetField(ViewModelLuaTable, PropertyName, Value);
		bPropertyWasSet = true;
	}

	// Broadcast property change only if property was actually set
	if (bPropertyWasSet)
	{
		LuaBroadcastFieldValueChanged(FName(*PropertyName));
	}
}

/**
 * @brief Calls a function defined in the ViewModel's Lua table, injecting the ViewModel as `self`.
 *
 * If the ViewModel table is not initialized or the named field is not a function, the call does not occur and a Lua `nil` is returned.
 *
 * @param Name Name of the function in the ViewModel Lua table.
 * @param Args Arguments to pass to the Lua function; the ViewModel table is inserted as the first argument (acts as `self`) before calling.
 * @return FLuaValue The value returned by the Lua function, or `nil` if the ViewModel table is uninitialized or the named function is not found.
 */
FLuaValue ULuaViewModelBridge::LuaCallFunction(const FString& Name, TArray<FLuaValue> Args)
{
	if (ViewModelLuaTable.Type != ELuaValueType::Table)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaViewModelBridge: ViewModel Lua table is not initialized"));
		}
		return ULuaBlueprintFunctionLibrary::LuaCreateNil();
	}

	FLuaValue FunctionValue = ULuaBlueprintFunctionLibrary::LuaTableGetField(ViewModelLuaTable, Name);
	
	if (ULuaBlueprintFunctionLibrary::LuaValueIsFunction(FunctionValue))
	{
		// Insert the ViewModel table as the first argument (self)
		Args.Insert(ViewModelLuaTable, 0);
		return ULuaBlueprintFunctionLibrary::LuaValueCall(FunctionValue, Args);
	}
	else if (bLogError)
	{
		UE_LOG(LogLuaMachine, Warning, TEXT("LuaViewModelBridge: Function '%s' not found in ViewModel table"), *Name);
	}

	return ULuaBlueprintFunctionLibrary::LuaCreateNil();
}

/**
 * @brief Retrieves a field value from the ViewModel's Lua table.
 *
 * Retrieves the field with the given name from the ViewModel Lua table. If the ViewModel table is not initialized, returns Lua `nil` (and logs an error when logging is enabled).
 *
 * @param Name The field name to read from the ViewModel Lua table.
 * @return FLuaValue The field's Lua value, or `nil` if the ViewModel table is uninitialized or the field does not exist.
 */
FLuaValue ULuaViewModelBridge::LuaGetField(const FString& Name)
{
	if (ViewModelLuaTable.Type != ELuaValueType::Table)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaViewModelBridge: ViewModel Lua table is not initialized"));
		}
		return ULuaBlueprintFunctionLibrary::LuaCreateNil();
	}

	return ULuaBlueprintFunctionLibrary::LuaTableGetField(ViewModelLuaTable, Name);
}

/**
 * @brief Set a field on the ViewModel's Lua table.
 *
 * If the ViewModel Lua table is not initialized, no change is made and an error is logged when logging is enabled.
 *
 * @param Name The field name to set on the Lua table.
 * @param Value The Lua value to assign to the field.
 */
void ULuaViewModelBridge::LuaSetField(const FString& Name, FLuaValue Value)
{
	if (ViewModelLuaTable.Type != ELuaValueType::Table)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaViewModelBridge: ViewModel Lua table is not initialized"));
		}
		return;
	}

	ULuaBlueprintFunctionLibrary::LuaTableSetField(ViewModelLuaTable, Name, Value);
}

/**
 * @brief Get the ULuaState associated with this bridge.
 *
 * Retrieves the engine-level ULuaState corresponding to the bridge's LuaState identifier.
 *
 * @return ULuaState* Pointer to the ULuaState for this bridge, or `nullptr` if no LuaState is set or it cannot be obtained.
 */
ULuaState* ULuaViewModelBridge::LuaViewModelGetState()
{
	if (!LuaState)
	{
		return nullptr;
	}

	return ULuaBlueprintFunctionLibrary::LuaGetState(this, LuaState);
}

/**
 * @brief Broadcasts a UI MVVM field-value-changed event for the specified field.
 *
 * @param FieldName Name of the field whose value changed.
 */
void ULuaViewModelBridge::LuaBroadcastFieldValueChanged(const FName& FieldName)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(FieldName);
}

/**
 * Attempts to call a global Lua function by name and store its result.
 *
 * If a global value with the given name is a callable Lua function, calls it with the provided arguments and stores the function's return value in OutResult.
 *
 * @param FunctionName Name of the global Lua function to call.
 * @param Args Arguments to pass to the Lua function.
 * @param OutResult Output parameter that receives the function's return value if the call is made.
 * @return `true` if a callable global function named FunctionName was found and invoked (OutResult was set), `false` otherwise.
 */
bool ULuaViewModelBridge::CallLuaFunctionIfExists(const FString& FunctionName, const TArray<FLuaValue>& Args, FLuaValue& OutResult)
{
	if (FunctionName.IsEmpty())
	{
		return false;
	}

	if (!LuaState)
	{
		if (bLogError)
		{
			UE_LOG(LogLuaMachine, Error, TEXT("LuaViewModelBridge: LuaState is not set"));
		}
		return false;
	}

	// Try to get the function from the global table
	FLuaValue FunctionValue = ULuaBlueprintFunctionLibrary::LuaGetGlobal(this, LuaState, FunctionName);
	
	if (ULuaBlueprintFunctionLibrary::LuaValueIsFunction(FunctionValue))
	{
		OutResult = ULuaBlueprintFunctionLibrary::LuaValueCall(FunctionValue, Args);
		return true;
	}
	else if (bLogError)
	{
		UE_LOG(LogLuaMachine, Warning, TEXT("LuaViewModelBridge: Lua function '%s' not found or not callable"), *FunctionName);
	}

	return false;
}