// Copyright 2018-2023 - Roberto De Ioris

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "LuaState.h"
#include "LuaValue.h"
#include "LuaViewModelBridge.generated.h"

/**
 * Bridge class that connects UMG ViewModels with Lua scripting
 * Allows Lua scripts to bind to and modify ViewModel properties
 */
UCLASS(Blueprintable, BlueprintType)
class /**
 * Bridge between UMG MVVM ViewModels and Lua scripting, exposing a Lua-backed table,
 * optional Lua hooks for property get/set, and utilities for calling Lua functions.
 */

/**
 * Initialize the Lua table backing this ViewModel and prepare any required Lua state.
 */
 
/**
 * Retrieve a value for a ViewModel property from the Lua table or via an optional Lua getter.
 * @param PropertyName Name of the property to retrieve.
 * @returns The value of the requested property as an FLuaValue, or an empty/default FLuaValue if not present.
 */

/**
 * Set a ViewModel property value in the Lua table and invoke an optional Lua setter.
 * @param PropertyName Name of the property to set.
 * @param Value Value to assign to the property.
 */

/**
 * Invoke a function defined on the ViewModel's Lua table and return its result.
 * @param Name Name of the Lua function to call.
 * @param Args Arguments to pass to the Lua function.
 * @returns The function's return value as an FLuaValue, or an empty/default FLuaValue if the call failed or no return value.
 */

/**
 * Get a field value directly from the ViewModel's Lua table.
 * @param Name Name of the field to read.
 * @returns The field's value as an FLuaValue, or an empty/default FLuaValue if the field does not exist.
 */

/**
 * Set a field on the ViewModel's Lua table.
 * @param Name Name of the field to set.
 * @param Value Value to assign to the field.
 */

/**
 * Return the ULuaState instance associated with this ViewModel, if available.
 * @returns Pointer to the active ULuaState, or nullptr if no state is available.
 */

/**
 * Notify Lua that a named field's value has changed on the ViewModel.
 * @param FieldName The name of the field that changed.
 */

/**
 * Retrieve the internal FLuaValue representing this ViewModel's Lua table.
 * @returns The ViewModel's Lua table as an FLuaValue.
 */

/**
 * Attempt to call a named Lua function if it exists on the ViewModel's table and capture its result.
 * @param FunctionName Name of the Lua function to invoke.
 * @param Args Arguments to pass to the function.
 * @param OutResult Out parameter set to the function's return value when invocation succeeds.
 * @returns `true` if the function existed and was called successfully, `false` otherwise.
 */
LUAMACHINE_API ULuaViewModelBridge : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	ULuaViewModelBridge();

	// Lua State to use for this ViewModel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	TSubclassOf<ULuaState> LuaState;

	// Lua table that represents this ViewModel's data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	TMap<FString, FLuaValue> Table;

	// Optional Lua function to call when a property is requested
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	FName OnGetPropertyLuaFunction;

	// Optional Lua function to call when a property is set
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	FName OnSetPropertyLuaFunction;

	// Enable logging of Lua errors
	UPROPERTY(EditAnywhere, Category = "Lua")
	bool bLogError;

	// Initialize the Lua table for this ViewModel
	UFUNCTION(BlueprintCallable, Category = "Lua")
	void InitializeLuaViewModel();

	// Get a property value from Lua
	UFUNCTION(BlueprintCallable, Category = "Lua")
	FLuaValue LuaGetProperty(const FString& PropertyName);

	// Set a property value in Lua
	UFUNCTION(BlueprintCallable, Category = "Lua")
	void LuaSetProperty(const FString& PropertyName, FLuaValue Value);

	// Set a property value in Lua using a pre-computed FName for performance
	void LuaSetProperty(const FName& PropertyName, FLuaValue Value);

	// Call a Lua function from the ViewModel's table
	UFUNCTION(BlueprintCallable, Category = "Lua", meta = (AutoCreateRefTerm = "Args"))
	FLuaValue LuaCallFunction(const FString& Name, TArray<FLuaValue> Args);

	// Get a field from the ViewModel's Lua table
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lua")
	FLuaValue LuaGetField(const FString& Name);

	// Set a field in the ViewModel's Lua table
	UFUNCTION(BlueprintCallable, Category = "Lua")
	void LuaSetField(const FString& Name, FLuaValue Value);

	// Get the internal Lua state instance
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lua")
	ULuaState* LuaViewModelGetState();

	// Broadcast property change from Lua
	UFUNCTION(BlueprintCallable, Category = "Lua")
	void LuaBroadcastFieldValueChanged(const FName& FieldName);

	// Get the ViewModel's Lua table
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lua")
	FLuaValue GetViewModelLuaTable() const { return ViewModelLuaTable; }

protected:
	// The Lua table value representing this ViewModel
	FLuaValue ViewModelLuaTable;

	// Helper to call a Lua function if it exists
	bool CallLuaFunctionIfExists(const FString& FunctionName, const TArray<FLuaValue>& Args, FLuaValue& OutResult);
};