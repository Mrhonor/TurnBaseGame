// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OrderInput.h"
#include "OrderProcessComponent.generated.h"

class UGridManagerComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class TURNBASEGAME_API UOrderProcessComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order", meta = (BlueprintProtected = "true"))
		FOrderInput CurrentExecuteOrder;

private:
	UPROPERTY(BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TArray<FOrderInput> OrderList;

	/**
	 * Max num of Order
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Order", meta = (AllowPrivateAccess = "true"))
		int32 MaxOrderNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
		bool bEnableBattleInput;

	UPROPERTY()
		bool bOrderExecuting;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "GameMode")
	UGridManagerComponent* CurrentGridManager;


public:	
	// Sets default values for this component's properties
	UOrderProcessComponent();

	/**
	 * Add Order
	 */
	UFUNCTION(BlueprintCallable, Category = "Order")
	void AddOrderInput(FOrderInput InputOrder);

	UFUNCTION(BlueprintCallable, Category = "Order")
		FORCEINLINE int32 GetMaxOrderNum() const { return MaxOrderNum; }

	UFUNCTION(BlueprintCallable, Category = "Order")
		FORCEINLINE int32 GetCurrentOrderNum() const { return OrderList.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Order")
		void SetMaxOrderNum(int32 NewNum);

	/**
	 * Execute the order in OrderList[0]
	 */
	UFUNCTION(BlueprintCallable, Category = "Order")
	void ExecuteFirstOrder();

	UFUNCTION(BlueprintCallable, Category = "Order")
		void ClearLatestOrder();

	UFUNCTION(BlueprintCallable, Category = "Order")
		FORCEINLINE FOrderInput GetLatestOrder() const {
		return OrderList.Num() ? OrderList[OrderList.Num() - 1] : FOrderInput();
	}

	UFUNCTION(BlueprintCallable, Category = "Order")
		FORCEINLINE FOrderInput GetFirstOrder() const {
		return OrderList.Num() ? OrderList[0] : FOrderInput();
	}

	UFUNCTION(BlueprintCallable, Category = "Input")
		void SetEnableBattleInput(bool State) { bEnableBattleInput = State; }

	UFUNCTION(BlueprintCallable, Category = "Input")
		FORCEINLINE bool GetEnableBattleInput() const { return bEnableBattleInput; }

	UFUNCTION(BlueprintCallable, Category = "Order")
		void SetOrderExecuting(bool State) { bOrderExecuting = State; }

	UFUNCTION(BlueprintCallable, Category = "Order")
		FORCEINLINE bool GetOrderExecuting() const { return bOrderExecuting; }

	UFUNCTION(BlueprintCallable, Category = "Order")
		bool CanAddOrderInput() const;

	UFUNCTION(BlueprintCallable, Category = "Order")
		FORCEINLINE UGridManagerComponent* GetCurrentGridManager() const { return CurrentGridManager; }

	UFUNCTION(BlueprintCallable, Category = "Order")
		FORCEINLINE FOrderInput GetCurrentExecuteOrder() const { return CurrentExecuteOrder; }

	UFUNCTION(BlueprintCallable, Category = "Order")
		void SetCurrentExecuteOrder(FOrderInput NewOrder) { CurrentExecuteOrder = NewOrder; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Order")
		void OnUpdateAttackState();


};
