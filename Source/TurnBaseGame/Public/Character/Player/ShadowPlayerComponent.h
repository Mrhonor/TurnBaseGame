// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ChildActorComponent.h"
#include "OrderInput.h"
#include "ShadowPlayerComponent.generated.h"

class ATurnBasePlayerCharacter;

/**
 * 
 */
UCLASS()
class TURNBASEGAME_API UShadowPlayerComponent : public UChildActorComponent
{
	GENERATED_BODY()
	
public:
	UShadowPlayerComponent();

private:
	bool bShadowActive;

public:

	virtual void OnRegister() override;

	UFUNCTION(BlueprintCallable, Category = "ShadowCharacter")
		FORCEINLINE bool GetShadowActive() const { return bShadowActive; }

	/**
	* SetActive
	* @param NewState   NewState of Active
	*/
	UFUNCTION(BlueprintCallable, Category = "ShadowCharacter")
		virtual void SetShadowActive(bool NewState);

	/**
	 * Order input
	 * @param Comment
	 * @return Comment
	 */
	UFUNCTION(BlueprintCallable, Category = "ShadowCharacter")
		void AddOrderInput(FOrderInput & InputOrder) const;

	/**
	 * Get the Shadow Actor
	 * @return the pointer of ATurnBasePlayerCharacter
	 */
	UFUNCTION(BlueprintCallable, Category = "ShadowCharacter")
		ATurnBasePlayerCharacter* GetShadowActor() const;

	/**
	 * make the Shadow Character back to the last order state
	 */
	UFUNCTION(BlueprintCallable, Category = "ShadowCharacter")
		virtual void ShadowBackspace();

};
