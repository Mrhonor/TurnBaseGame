// Fill out your copyright notice in the Description page of Project Settings.


#include "AddGridMovementInput.h"

void UAddGridMovementInputAsync::OnCompleteHandle()
{
	OnComplete.Broadcast();
	SetReadyToDestroy();
}

void UAddGridMovementInputAsync::OnInterruptedHandle()
{
	OnInterrupted.Broadcast();
	SetReadyToDestroy();
}

void UAddGridMovementInputAsync::Activate()
{
	CurrentCharacter->AddGridMovementInput(GoalLocation);
	CompletedHandle.BindUObject(this, &UAddGridMovementInputAsync::OnCompleteHandle);
	CurrentCharacter->SendCompletedHandle(CompletedHandle);

	InterruptedHandle.BindUObject(this, &UAddGridMovementInputAsync::OnInterruptedHandle);
	CurrentCharacter->SendInterruptedHandle(InterruptedHandle);

}

UAddGridMovementInputAsync* UAddGridMovementInputAsync::AddGridMovementInput(ATurnBaseCharacter* MoveCharacter, const FVector& TargetLocation)
{
	UAddGridMovementInputAsync* BlueprintNode = NewObject<UAddGridMovementInputAsync>();
	BlueprintNode->CurrentCharacter = MoveCharacter;
	BlueprintNode->GoalLocation = TargetLocation;

	return BlueprintNode;
}


