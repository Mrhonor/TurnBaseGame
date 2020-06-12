// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TurnBaseCharacter.h"
#include "PlayerCameraPawn.h"
#include "InputBridge.h"
#include "TurnBasePlayerCharacter.generated.h"

class UGridManagerComponent;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ShadowCharacter", meta = (BlueprintProtected = "true"))
		class UShadowPlayerComponent* ShadowPlayerComponent;

	virtual void PossessedBy(AController* controller) override;
	virtual void UnPossessed() override;
	
private:
	// is it controlled by PlayerController
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
		bool bControlled;

	FTransform CameraTransform;


public:
	ATurnBasePlayerCharacter();

	// virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	UFUNCTION(BlueprintCallable, Category = Camera)
		FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	UFUNCTION(BlueprintCallable, Category = Camera)
		FORCEINLINE class UCameraComponent* GetTopDownCamera() const { return TopDownCamera;}

	// set this pawn control state
	UFUNCTION(BlueprintCallable, Category = "Input")
		void OnControl(bool ControlState) { bControlled = ControlState; }

	UFUNCTION(BlueprintCallable, Category = "Control")
		virtual void ExitControl();

	// Reset the camera to the preview location
	UFUNCTION(BlueprintCallable, Category = Camera)
		void ResetCamera();

	/**
	 * get the ordet executing state
	 */
	UFUNCTION(BlueprintCallable, Category = "Order")
		FORCEINLINE bool GetOrderExecuting() const { return OrderProcessComponent->GetOrderExecuting(); }


	void DestroyShadowCharacter();

	// move the camera base on mouse cursor
	virtual void CameraMove(float XValue, float YValue) override;

	void BackspacePressed();

protected:
	virtual void BeginPlay() override;

	// handle for mouse wheel roll
	UFUNCTION(BlueprintCallable, Category = "Control", meta = (BlueprintProtected = "true"))
		void MouseWheelRoll();

	// consume mouse right input -- move to given place
	UFUNCTION(BlueprintCallable, Category = "Control", meta = (BlueprintProtected = "true"))
		void ConsumeMouseRightInput();

	/**
	 * Called when any key pressed
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Input", meta = (BlueprintProtected = "true"))
	void KeyPressed(FKey key);
	virtual void KeyPressed_Implementation(FKey key);

	/**
	 * Called when any key pressed
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Input", meta = (BlueprintProtected = "true"))
	void KeyReleased(FKey key);
	virtual void KeyReleased_Implementation(FKey key);

	virtual void OrderBackspace() override;

	virtual void OnGameStateChangeDelegate(ETurnBasePlayState NewState) override;

};
