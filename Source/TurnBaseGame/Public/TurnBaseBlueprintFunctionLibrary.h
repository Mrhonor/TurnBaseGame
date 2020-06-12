// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TurnBaseBlueprintFunctionLibrary.generated.h"


/**
 * 
 */
UCLASS()
class TURNBASEGAME_API UTurnBaseBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "Library")
		static FVector CalcMouseLocation(FVector Location, FVector Direction);
};
