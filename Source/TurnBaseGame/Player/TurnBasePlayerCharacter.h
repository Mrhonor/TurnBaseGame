// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TurnBaseGame/Character/TurnBaseCharacter.h"
#include "PlayerCameraPawn.h"
#include "TurnBasePlayerCharacter.generated.h"


/**
 * 
 */
UCLASS()
class TURNBASEGAME_API ATurnBasePlayerCharacter : public ATurnBaseCharacter
{
	GENERATED_BODY()

public:
	// storage player input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FPlayerCameraInput PlayerInputMessage;

protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (BlueprintProtected = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (BlueprintProtected = "true"))
		class UCameraComponent* TopDownCamera;

private:
	// is it controlled by PlayerController
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
		bool bControlled;

public:
	ATurnBasePlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	UFUNCTION(BlueprintCallable, Category = Camera)
		FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	UFUNCTION(BlueprintCallable, Category = Camera)
		FORCEINLINE class UCameraComponent* GetTopDownCamera() const { return TopDownCamera;}

	// set this pawn controll state
	UFUNCTION(BlueprintCallable, Category = "Input")
		void OnControl(bool ControlState) { bControlled = ControlState; }

	UFUNCTION(BlueprintCallable, Category = "Control")
		virtual void ExitControl();


protected:
	virtual void BeginPlay() override;

	// handle for mouse wheel roll
	UFUNCTION(BlueprintCallable, Category = "Control", meta = (BlueprintProtected = "true"))
		void MouseWheelRoll();

	// consume mouse right input -- move to given place
	UFUNCTION(BlueprintCallable, Category = "Control", meta = (BlueprintProtected = "true"))
		void ConsumeMouseRightInput();
};
