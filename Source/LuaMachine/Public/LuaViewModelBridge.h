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
class LUAMACHINE_API ULuaViewModelBridge : public UMVVMViewModelBase
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
	FString OnGetPropertyLuaFunction;

	// Optional Lua function to call when a property is set
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lua")
	FString OnSetPropertyLuaFunction;

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
