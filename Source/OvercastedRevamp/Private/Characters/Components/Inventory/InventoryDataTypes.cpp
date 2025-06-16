// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/Inventory/InventoryDataTypes.h"


FName UInventoryDataTypes::FindItemTag(TArray<FItemTags> Tags , EItemTags Key)
{
	for (FItemTags& Tag : Tags)
	{
		if (Tag.Key == Key)
		{
			return Tag.Value;
		}
	}
	
	return "";
}

bool UInventoryDataTypes::DoesItemHaveTag(TArray<FItemTags> Tags,const EItemTags Key)
{
	for (const FItemTags Tag : Tags)
	{
		if (Tag.Key == Key)
		{
			return true;
		}
	}
	return false;
}
