// Copyright 2018-2023 - Roberto De Ioris

#include "LuaViewModelBridge.h"
#include "LuaBlueprintFunctionLibrary.h"

ULuaViewModelBridge::ULuaViewModelBridge()
{
	bLogError = true;
}

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
			// If the function returns true (as a bool), it handled the property
			if (ULuaBlueprintFunctionLibrary::LuaValueIsBoolean(Result) && 
			    ULuaBlueprintFunctionLibrary::Conv_LuaValueToBool(Result))
			{
				bPropertyWasSet = true;
			}
			else
			{
				// Function returned false or non-bool, use default behavior
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

ULuaState* ULuaViewModelBridge::LuaViewModelGetState()
{
	if (!LuaState)
	{
		return nullptr;
	}

	return ULuaBlueprintFunctionLibrary::LuaGetState(this, LuaState);
}

void ULuaViewModelBridge::LuaBroadcastFieldValueChanged(const FName& FieldName)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(FieldName);
}

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
