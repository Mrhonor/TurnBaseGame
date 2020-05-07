// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TurnBasePlayerController.h"
#include "PlayerCameraPawn.generated.h"



UCLASS()
class TURNBASEGAME_API APlayerCameraPawn : public APawn
{
	GENERATED_BODY()


public:
	// Sets default values for this pawn's properties
	APlayerCameraPawn();

	// storage player input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FPlayerCameraInput PlayerInputMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		class UCameraComponent* MainCamera;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	// Camera move speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float CameraMoveSpeed;

	// is it controlled by PlayerController
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
		bool bControlled;

	// transient storage the actor cursor hit
	UPROPERTY(transient)
		AActor* CursorHitActor;

	// check whether or not left mouse key press and release hit the same actor.
	bool StartHit;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Get the Max camera move speed
	UFUNCTION(BlueprintPure, Category = Camera)
		FORCEINLINE float GetCameraMoveSpeeds() const { return CameraMoveSpeed; }

	// is it this pawn controlled by PlayerControlled
	UFUNCTION(BlueprintPure, Category = "Input")
		FORCEINLINE bool IsControlling() const { return bControlled; }

	// set this pawn controll state
	UFUNCTION(BlueprintCallable, Category = "Input")
		void OnControl(bool ControlState) { bControlled = ControlState; }

	UFUNCTION(BlueprintCallable, Category = "Control")
		virtual void ExitControl();

protected:
	/** handle for Mouse Left key 
	*	@param Hit  null
	*   @return null
	*/
	UFUNCTION(BlueprintCallable, Category = "Control", meta = (BlueprintProtected = "true"))
		bool MouseLeftKeyProcess(const FHitResult& Hit);

	// handle for mouse wheel roll
	UFUNCTION(BlueprintCallable, Category = "Control", meta = (BlueprintProtected = "true"))
		void MouseWheelRoll();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	// the function use to unpossess this pawn and empty the input
	UFUNCTION(BlueprintCallable, Category = "Input", meta = (AllowPrivateAccess = "true"))
		void ResetPlayerControl();

	UFUNCTION(BlueprintCallable, Category = "Control", meta = (AllowPrivateAccess = "true"))
		void ChangeControl();
};
