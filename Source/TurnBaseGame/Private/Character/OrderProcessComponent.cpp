// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderProcessComponent.h"
#include "GridManagerComponent.h"
#include "TurnBaseCharacter.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"

// Sets default values for this component's properties
UOrderProcessComponent::UOrderProcessComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MaxOrderNum = 6;
	bOrderExecuting = false;
	bEnableBattleInput = false;
	// ...
}


void UOrderProcessComponent::AddOrderInput(FOrderInput InputOrder)
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

void UOrderProcessComponent::ExecuteFirstOrder()
{
	if (OrderList.Num() > 0 && !bOrderExecuting) {
		bOrderExecuting = true;
		CurrentExecuteOrder = OrderList[0];
		IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(GetOwner());
		AbilityInterface->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(OrderList[0].OrderTagType);

		OrderList.RemoveAt(0);
	}
}

void UOrderProcessComponent::ClearLatestOrder()
{
	if (OrderList.Num() > 0) {
		OrderList.RemoveAt(OrderList.Num() - 1);
	}
}

bool UOrderProcessComponent::CanAddOrderInput() const
{
	return bEnableBattleInput && OrderList.Num() < MaxOrderNum;
}

void UOrderProcessComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentGridManager = GetWorld()->GetAuthGameMode()->FindComponentByClass<UGridManagerComponent>();
}

