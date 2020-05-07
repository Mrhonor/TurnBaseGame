// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TurnBasePlayerController.generated.h"

class ATurnBaseGameModeBase;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Control", meta = (BlueprintProtected = "true"))
		TSubclassOf<class APlayerCameraPawn> CameraPawn;

private:
	// get access to CurrentGameMode
	ATurnBaseGameModeBase* CurrentGameMode;

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
	void CameraOut();
	void Battle();
	void Battling();
	// End for handling
	/* ****************************************** */

	virtual void BeginPlay() override;	
	virtual void OnPossess(APawn *InPawn) override;
	virtual void OnUnPossess() override;
};
