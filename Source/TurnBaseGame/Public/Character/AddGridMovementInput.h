// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "TurnBaseCharacter.h"
#include "AddGridMovementInput.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridMovement);

/**
 * 
 */
UCLASS()
class TURNBASEGAME_API UAddGridMovementInputAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FOnGridMovement OnComplete;

	UPROPERTY(BlueprintAssignable)
		FOnGridMovement OnInterrupted;

private:
	UPROPERTY()
	ATurnBaseCharacter* CurrentCharacter;

	UPROPERTY()
	FVector GoalLocation;

	FCompleteDelegate CompletedHandle;

	FInterruptedDelegate InterruptedHandle;
public:
	void OnCompleteHandle();

	void OnInterruptedHandle();

	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Grid", meta = (DefaultToSelf = "MoveCharacter", BlueprintInternalUseOnly = "true"))
		static UAddGridMovementInputAsync* AddGridMovementInput(ATurnBaseCharacter* MoveCharacter, const FVector& TargetLocation);
//
//private:
//	void ExecuteAction
};
