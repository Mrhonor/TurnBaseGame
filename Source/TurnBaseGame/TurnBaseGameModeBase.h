// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TurnBaseGameModeBase.generated.h"

class AGridScene;
class ATurnBaseCharacter;

UENUM(BlueprintType)
enum ETurnBasePlayState {
	EPlaying,
	EBattle,
	EBattling,
	EBattleWon,
	EBattleLose,
	EUnknow
};


/**
 * 
 */
UCLASS()
class TURNBASEGAME_API ATurnBaseGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

protected:

	virtual void BeginPlay() override;

	// the HUD widget when the game state is EBattle.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Battle", meta = (BlueprintProtected = "true"))
		TSubclassOf<UUserWidget> HUDWidgetBattle;

	UPROPERTY()
		UUserWidget *CurrentWidget;

private:
	// storage the type of GridScene, Grid will spawn under the EBattle State and Destory when Battle win or lose. 
	UPROPERTY(EditDefaultsOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AGridScene> GridScene;

	// Spawned Grid 
	AGridScene* SpawnedGrid;

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
		FORCEINLINE AGridScene* GetSpawnedGrid() const { return SpawnedGrid; }

	/** Show the Section on the screen
	* @param  ShowLocation		The location want to show
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid")
		void ShowSelectSection(const FVector &ShowLocation);

	/** Start to expose AGridScene interface **/
	UFUNCTION(BlueprintCallable, Category = "Grid")
		bool MoveObjectTo(AActor* MovedObject, const FVector &Location, bool IsCenter = true, bool IsMove = true);

	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		bool MoveCharacterTo(ATurnBaseCharacter* MovedCharacter, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		bool UpdateGrid(ATurnBaseCharacter* MovedCharacter, const FVector& Location);
	/** End to expose AGridScene interface **/
};
