// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderProcessComponent.h"
#include "TurnBaseCharacter.h"

// Sets default values for this component's properties
UOrderProcessComponent::UOrderProcessComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MaxOrderNum = 3;
	bOrderExecuting = false;
	bEnableBattleInput = false;
	// ...
}

// Called every frame
void UOrderProcessComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UOrderProcessComponent::AddOrderInput(FOrderInput &InputOrder)
{
	if (OrderList.Num() < MaxOrderNum) {
		OrderList.Add(InputOrder);
	}
}

void UOrderProcessComponent::SetMaxOrderNum(int32 NewNum)
{
	if (NewNum > OrderList.Num()) {
		MaxOrderNum = NewNum;
	}
}

void UOrderProcessComponent::ExecutrFirstOrder()
{
	if (OrderList.Num() > 0 && !bOrderExecuting) {
		switch (OrderList[0].OrderType) {
		case EMoveOrder:
			if (ATurnBaseCharacter* TestCharacter = Cast<ATurnBaseCharacter>(GetOwner())) {
				TestCharacter->AddGridMovementInput(OrderList[0].TargetLocation);
			}
			bOrderExecuting = true;
			OrderList.RemoveAt(0);
			break;
		default:
			break;
		}
	}
}

void UOrderProcessComponent::ClearLatestOrder()
{
	if (OrderList.Num() > 0) {

	}
}

bool UOrderProcessComponent::CanAddOrderInput() const
{
	return bEnableBattleInput && OrderList.Num() < MaxOrderNum;
}

