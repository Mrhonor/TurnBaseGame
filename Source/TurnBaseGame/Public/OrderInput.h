// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "OrderInput.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EOrderType : uint8
{
	EMoveOrder UMETA(DisplayName = "Move"),
	EAttackOrder UMETA(DisplayName = "Attack"),
	EUnknowOrder UMETA(DisplayName = "UnknowOrder")
};

UENUM(BlueprintType)
enum class ETurnBasePlayState: uint8
{
	EPlaying UMETA(DisplayName = "Playing"),
	EBattle UMETA(DisplayName = "Battle"),
	EBattlePrepare UMETA(DisplayName = "BattlePrepare"),
	EBattling UMETA(DisplayName = "Battling"),
	EBattleWon UMETA(DisplayName = "BattleWon"),
	EBattleLose UMETA(DisplayName = "BattleLose"),
	EUnknow UMETA(DisplayName = "UnknowGameState")
};

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	EIdle UMETA(DisplayName = "Idle"),
	EAttack UMETA(DisplayName = "Attack"),
};


USTRUCT(BlueprintType)
struct FOrderInput
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Order")
	EOrderType OrderType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Order")
	FVector TargetLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Order")
	FVector CurrentLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Order")
	FGameplayTagContainer OrderTagType;

	FOrderInput();
	FOrderInput(const EOrderType& order, const FVector& targetLocation, const FVector& currentLocation);
	FOrderInput(const EOrderType& order, const FVector& targetLocation, const FVector& currentLocation, const FGameplayTagContainer& orderTagType);
};
