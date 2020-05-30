// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OrderInput.h"
#include "TurnBaseGameModeBase.generated.h"

class AGridScene;
class ATurnBaseCharacter;
class UGridManagerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameStateChange, ETurnBasePlayState, CurrentGameState);

/**
 * 
 */
UCLASS()
class TURNBASEGAME_API ATurnBaseGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	FGameStateChange OnGameStateChange;

protected:

	virtual void BeginPlay() override;

	// the HUD widget when the game state is EBattle.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle", meta = (BlueprintProtected = "true"))
		TSubclassOf<UUserWidget> HUDWidgetBattle;

	UPROPERTY()
		UUserWidget *CurrentWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		UGridManagerComponent* GridManagerComponent;


private:

	// current game state
	ETurnBasePlayState CurrentGameState;

public:
	ATurnBaseGameModeBase();

	// return CurrentGameState
	UFUNCTION(BlueprintPure, Category = "GameMode")
		FORCEINLINE ETurnBasePlayState GetCurrentGameState() const { return CurrentGameState; }

	UFUNCTION(BlueprintCallable, Category = "GameMode")
		void SetCurrentGameState(ETurnBasePlayState NewState);

	UFUNCTION(BlueprintPure, Category = "Grid")
		FORCEINLINE UGridManagerComponent* GetGridManagerComponent() const { return GridManagerComponent; }

};
