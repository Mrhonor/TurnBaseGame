// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TurnBasePlayerController.generated.h"

USTRUCT(BlueprintType)
struct FPlayerCameraInput {
	GENERATED_USTRUCT_BODY()
public:
	// For the Pawn movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FVector2D MovementInput;

	// Is press Left mouse button
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		bool bMouseLeft;

	// Is press Right mouse button
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		bool bMouseRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FHitResult CursorHit;
	
	// empty all the things.
	void Empty() {
		MovementInput = FVector2D::ZeroVector;
		bMouseLeft = bMouseRight = false;
		CursorHit.Reset();
	}
};

/**
 * 
 */
UCLASS()
class TURNBASEGAME_API ATurnBasePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	FPlayerCameraInput* PlayerInputMessagePtr;

protected:


public:
	ATurnBasePlayerController();

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	// To Change this Controller controlled pawn
	UFUNCTION(BlueprintCallable, Category = "Control")
		void ChangeControlPawn(APawn *InPawn);

protected:
	/* ***************************************** */
	// Start to Handle Player Input
	void OnClickMouseLeft();
	void OnClickMouseRight();
	void FinishClickMouseLeft();
	void FinishClickMouseRight();
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void Zoom(float AxisValue);
	// End for handling
	/* ****************************************** */

	virtual void OnPossess(APawn *InPawn) override;
	virtual void OnUnPossess() override;
};
