// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TurnBaseGame/Scene/GridPropertyInterface.h"
#include "TurnBaseCharacter.generated.h"

UCLASS()
class TURNBASEGAME_API ATurnBaseCharacter : public ACharacter, public IGridPropertyInterface
{
	GENERATED_BODY()

protected:
	FVector GridTargetLocation;
	FVector PreGridLocation;
	bool IsNeedToMove;

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


	UFUNCTION(BlueprintCallable, Category = "Input")
		virtual void AddGridMovementInput(const FVector& TargetLocation) { GridTargetLocation = TargetLocation; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Input")
		virtual bool MoveToTargetLocation();
};
