// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OrderInput.h"
#include "GridManagerComponent.generated.h"

class AGridScene;
class ATurnBaseCharacter;
class ATurnBaseGameModeBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class TURNBASEGAME_API UGridManagerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	friend ATurnBaseGameModeBase;

private:
	// storage the type of GridScene, Grid will spawn under the EBattle State and Destroy when Battle win or lose. 
	UPROPERTY(EditDefaultsOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AGridScene> GridScene;

	// Spawned Grid 
	AGridScene* SpawnedGrid;

	ETurnBasePlayState CurrentGameState;

public:	
	// Sets default values for this component's properties
	UGridManagerComponent();

	UFUNCTION(BlueprintPure, Category = "Grid")
		FORCEINLINE AGridScene* GetSpawnedGrid() const { return SpawnedGrid; }

	/** Show the Section on the screen
	* @param  ShowLocation		The location want to show
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid")
		void ShowSelectSection(const FVector &ShowLocation);

	UFUNCTION(BlueprintCallable, Category = "Order")
		bool OrderValidity(ATurnBaseCharacter* Character, const FOrderInput &Order);

	/** Start to expose AGridScene interface **/
	UFUNCTION(BlueprintCallable, Category = "Grid")
		FVector GetGridCenter(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Grid")
		bool MoveObjectTo(AActor* MovedObject, const FVector &Location, bool IsCenter = true, bool IsMove = true);

	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		bool MoveCharacterTo(ATurnBaseCharacter* MovedCharacter, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		bool UpdateGrid(ATurnBaseCharacter* MovedCharacter, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Order")
		FOrderInput GetLatestOrder(ATurnBaseCharacter* TheCharacter);

	UFUNCTION(BlueprintCallable, Category = "Order")
		void ClearLatestOrder(ATurnBaseCharacter* TheCharacter);
	/** End to expose AGridScene interface **/

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/**
	 * Spawn the Grid and Add the Actor into the Grid
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid")
		virtual bool GridSpawn();


private:
	UFUNCTION()
	void OnGameStateChangeDelegate(ETurnBasePlayState NewState);
		
};
