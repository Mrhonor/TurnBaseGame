// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrderInput.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum EOrderType {
	EMoveOrder UMETA(DisplayName = "Move"),
	EAttackOrder UMETA(DisplayName = "Attack"),
	EUnknowOrder UMETA(DisplayName = "UnknowOrder")
};

UENUM(BlueprintType)
enum ETurnBasePlayState {
	EPlaying UMETA(DisplayName = "Playing"),
	EBattle UMETA(DisplayName = "Battle"),
	EBattlePrepare UMETA(DisplayName = "BattlePrepare"),
	EBattling UMETA(DisplayName = "Battling"),
	EBattleWon UMETA(DisplayName = "BattleWon"),
	EBattleLose UMETA(DisplayName = "BattleLose"),
	EUnknow UMETA(DisplayName = "UnknowGameState")
};

USTRUCT(BlueprintType)
struct FOrderInput
{
	GENERATED_USTRUCT_BODY()

public:
	EOrderType OrderType;
	FVector TargetLocation;
	FVector CurrentLocation;

	FOrderInput();
	FOrderInput(const EOrderType& order, const FVector& targetLocation, const FVector& currentLocation);
};
