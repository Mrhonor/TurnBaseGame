// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GridPropertyInterface.h"
#include "OrderInput.h"
#include "OrderProcessComponent.h"
#include "TurnBaseCharacter.generated.h"


UCLASS()
class TURNBASEGAME_API ATurnBaseCharacter : public ACharacter, public IGridPropertyInterface
{
	GENERATED_BODY()
public:


protected:
	UPROPERTY(BlueprintReadWrite, Category = "Order")
		UOrderProcessComponent* OrderProcessComponent;

	// use to storage the destination of character movement 
	UPROPERTY(BlueprintReadWrite, Category = "Move", meta = (BlueprintProtected = "true"))
		FVector GridTargetLocation;

	// use to storage the previous location
	UPROPERTY(BlueprintReadOnly, Category = "Move", meta = (BlueprintProtected = "true"))
		FVector PreGridLocation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Move", meta = (BlueprintProtected = "true"))
		bool bNeedToMove;

	ETurnBasePlayState CurrentGameState;
public:
	// Sets default values for this character's properties
	ATurnBaseCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Begin GridPropertyInterface
	virtual bool IsBlockTheCharacter() const override { return true; }
	virtual void CharacterStandAboveEffect(class APawn* InPawn) override {}
	virtual FVector2D GetLengthAndWidth() const override { return FVector2D::ZeroVector; }
	// End GridPropertyInterface

	UFUNCTION(BlueprintCallable, Category = "Control")
		virtual void CameraMove(float XValue, float YValue);

	UFUNCTION(BlueprintCallable, Category = "Input")
		virtual void AddGridMovementInput(const FVector& TargetLocation) { GridTargetLocation = TargetLocation; }

	UFUNCTION(BlueprintCallable, Category = "Order")
		virtual UOrderProcessComponent* GetOrderProcessComponent() const { return OrderProcessComponent; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Input")
		virtual bool MoveToTargetLocation(bool bForce = false);

	// backspacing the order
	UFUNCTION(BlueprintCallable, Category = "Order", meta = (BlueprintProtected = "true"))
		virtual void OrderBackspace();

	UFUNCTION()
		virtual void OnGameStateChangeDelegate(ETurnBasePlayState NewState);
};
