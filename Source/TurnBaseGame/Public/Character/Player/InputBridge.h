// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputBridge.generated.h"
/**
 * A bridge for input transport among the pawn and controller
 */
USTRUCT(BlueprintType)
struct FPlayerCameraInput {
	GENERATED_USTRUCT_BODY()
public:
	// For the Pawn movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
		FVector2D MovementInput;

	// Is press Left mouse button
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
		bool bMouseLeft;

	// Is press Right mouse button
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
		bool bMouseRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
		FHitResult CursorHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
		float MouseWheelValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
		FVector MouseLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
		FVector MouseDirection;

	// empty all the things.
	void Empty() {
		MovementInput = FVector2D::ZeroVector;
		MouseLocation = FVector::ZeroVector;
		MouseDirection = FVector::ZeroVector;
		bMouseLeft = bMouseRight = false;
		CursorHit.Reset();
	}
};
