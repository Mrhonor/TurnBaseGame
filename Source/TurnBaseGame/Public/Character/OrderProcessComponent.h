// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OrderInput.h"
#include "OrderProcessComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TURNBASEGAME_API UOrderProcessComponent : public UActorComponent
{
	GENERATED_BODY()

public:	


private:
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

public:	
	// Sets default values for this component's properties
	UOrderProcessComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Add Order
	 */
	UFUNCTION(BlueprintCallable, Category = "MyProject")
	void AddOrderInput(FOrderInput &InputOrder);

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
		virtual void ExecutrFirstOrder();

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
};
