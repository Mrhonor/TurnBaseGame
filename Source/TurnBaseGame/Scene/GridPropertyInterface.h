// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridPropertyInterface.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UGridPropertyInterface : public UInterface
{
	GENERATED_BODY()
};


class IGridPropertyInterface
{
	GENERATED_BODY()
public:
	virtual bool IsBlockTheCharacter() const = 0;

	virtual void CharacterStandAboveEffect(class APawn* InPawn) = 0;

	virtual FVector2D GetLengthAndWidth() const = 0;
};
