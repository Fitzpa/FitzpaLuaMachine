// Copyright 2018-2023 - Roberto De Ioris

#include "LuaCommonUIWidget.h"
#include "LuaBlueprintFunctionLibrary.h"

ULuaCommonUIWidget::ULuaCommonUIWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bLogError = true;
}

void ULuaCommonUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeLuaTable();
	CallLuaFunctionIfExists(OnConstructedLuaFunction);
}

void ULuaCommonUIWidget::NativeDestruct()
{
	CallLuaFunctionIfExists(OnDestructedLuaFunction);
	
	Super::NativeDestruct();
}

void ULuaCommonUIWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	CallLuaFunctionIfExists(OnActivatedLuaFunction);
	OnLuaActivated.Broadcast();
}

void ULuaCommonUIWidget::NativeOnDeactivated()
{
	CallLuaFunctionIfExists(OnDeactivatedLuaFunction);
	OnLuaDeactivated.Broadcast();

	Super::NativeOnDeactivated();
}

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

bool ULuaCommonUIWidget::CallLuaFunctionIfExists(const FString& FunctionName)
{
	if (FunctionName.IsEmpty())
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

	// Try to get the function from the global table
	FLuaValue FunctionValue = ULuaBlueprintFunctionLibrary::LuaGetGlobal(this, LuaState, FunctionName);
	
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
		UE_LOG(LogLuaMachine, Warning, TEXT("LuaCommonUIWidget: Lua function '%s' not found or not callable"), *FunctionName);
	}

	return false;
}

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

ULuaState* ULuaCommonUIWidget::LuaWidgetGetState()
{
	if (!LuaState)
	{
		return nullptr;
	}

	return ULuaBlueprintFunctionLibrary::LuaGetState(this, LuaState);
}
