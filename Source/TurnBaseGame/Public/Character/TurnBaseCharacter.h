// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GridPropertyInterface.h"
#include "OrderInput.h"
#include "OrderProcessComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BaseAttributeSet.h"
#include "TurnBaseCharacter.generated.h"

DECLARE_DELEGATE(FCompleteDelegate);
DECLARE_DELEGATE(FInterruptedDelegate);

class UAddGridMovementInputAsync;

UCLASS()
class TURNBASEGAME_API ATurnBaseCharacter : public ACharacter, public IGridPropertyInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
		TArray <TSubclassOf<class UGameplayAbility>> GameplayAbilities;

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	FCompleteDelegate CompletedHandle;

	FInterruptedDelegate InterruptedHandle;

protected:
	/** List of attributes modified by the ability system */
	UPROPERTY()
		UBaseAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (BlueprintProtected = "true"))
		UAbilitySystemComponent* AbilitySystemComponent;

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

	UPROPERTY(BlueprintReadOnly, Category = "GameMode")
	ETurnBasePlayState CurrentGameState;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerState")
	EPlayerState CurrentPlayerState;
		
	UAddGridMovementInputAsync* CurrentGridMovementFunction;

private:
	bool bAbilitiesInitialized;

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

	virtual void AddGridMovementInput(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Grid", meta = (BlueprintInternalUseOnly = "true"))
		static UAddGridMovementInputAsync* AddGridMovementInputFunc(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Order")
		FORCEINLINE UOrderProcessComponent* GetOrderProcessComponent() const { return OrderProcessComponent; }

	UFUNCTION(BlueprintCallable, Category = Abilities)
		FORCEINLINE UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }

	/** Returns current health, will be 0 if dead */
	UFUNCTION(BlueprintCallable)
		virtual float GetHealth() const;

	/** Returns maximum health, health will never be greater than this */
	UFUNCTION(BlueprintCallable)
		virtual float GetMaxHealth() const;

	/** Returns current health, will be 0 if dead */
	UFUNCTION(BlueprintCallable)
		virtual int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
		virtual void SetPlayerState(EPlayerState NewState);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	FORCEINLINE EPlayerState GetPlayerState() const {return CurrentPlayerState;}

	void SendCompletedHandle(FCompleteDelegate &InOnCompleteDelegate) { CompletedHandle = InOnCompleteDelegate;}

	void SendInterruptedHandle(FInterruptedDelegate &InOnInterruptedDelegate) { InterruptedHandle = InOnInterruptedDelegate; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* controller) override;

	UFUNCTION(BlueprintCallable, Category = "Input")
		virtual bool MoveToTargetLocation(bool bForce = false);

	// backspacing the order
	UFUNCTION(BlueprintCallable, Category = "Order", meta = (BlueprintProtected = "true"))
		virtual void OrderBackspace();

	/**
	 * Called when health is changed, either from healing or from being damaged
	 * For damage this is called in addition to OnDamaged/OnKilled
	 *
	 * @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Attribute", meta = (BlueprintProtected = "true"))
		void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UFUNCTION()
		virtual void OnGameStateChangeDelegate(ETurnBasePlayState NewState);

	virtual void AddStartupGameplayAbilities();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attribut")
		void OnCollisionChange(bool NewState);
};
