// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderInput.h"

FOrderInput::FOrderInput()
	: OrderType(EOrderType::EUnknowOrder)
	, TargetLocation(FVector::ZeroVector)
	, CurrentLocation(FVector::ZeroVector)
{
}

FOrderInput::FOrderInput(const EOrderType & order, const FVector & targetLocation, const FVector &currentLocation)
	: OrderType(order)
	, TargetLocation(targetLocation)
	, CurrentLocation(currentLocation)
{
}

FOrderInput::FOrderInput(const EOrderType& order, const FVector& targetLocation, const FVector& currentLocation, const FGameplayTagContainer& orderTagType)
	: OrderType(order)
	, TargetLocation(targetLocation)
	, CurrentLocation(currentLocation)
	, OrderTagType(orderTagType)
{
}
