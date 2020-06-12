// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBaseBlueprintFunctionLibrary.h"

FVector UTurnBaseBlueprintFunctionLibrary::CalcMouseLocation(FVector Location, FVector Direction)
{
	return Location - Direction * (Location.Z / Direction.Z);
}
