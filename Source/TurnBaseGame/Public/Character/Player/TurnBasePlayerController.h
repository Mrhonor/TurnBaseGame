// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputBridge.h"
#include "OrderInput.h"
#include "TurnBasePlayerController.generated.h"

class UGridManagerComponent;
class ATurnBaseGameModeBase;

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

	// get access to CurrentGridManager
	UGridManagerComponent* CurrentGridManager;

	ETurnBasePlayState CurrentGameState;

	ATurnBaseGameModeBase* CurrentGameMode;

private:



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
	void Backspace();
	// End for handling
	/* ****************************************** */

	virtual void BeginPlay() override;	
	virtual void OnPossess(APawn *InPawn) override;
	virtual void OnUnPossess() override;

private:
	float ScreenMoveMouseClampRange(float MouseValue);

	UFUNCTION()
		virtual void OnGameStateChangeDelegate(ETurnBasePlayState NewState);
};
