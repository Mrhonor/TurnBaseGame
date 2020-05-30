// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowPlayerComponent.h"
#include "OrderProcessComponent.h"
#include "TurnBasePlayerCharacter.h"

UShadowPlayerComponent::UShadowPlayerComponent()
{
	bShadowActive = false;
}

void UShadowPlayerComponent::OnRegister()
{
	Super::OnRegister();

	DestroyChildActor();
}

void UShadowPlayerComponent::SetShadowActive(bool NewState)
{
	if (NewState == bShadowActive) return;
	bShadowActive = NewState;
	if (bShadowActive)
	{
		// Active func
		CreateChildActor();
		GetChildActor()->SetActorTransform(GetOwner()->GetActorTransform());
	} 
	else
	{
		// deActive func
		DestroyChildActor();
	}
	
}

void UShadowPlayerComponent::AddOrderInput(FOrderInput & InputOrder) const
{
	UOrderProcessComponent* CurrentOrderComponent = GetChildActor()->FindComponentByClass<UOrderProcessComponent>();
	if (bShadowActive && CurrentOrderComponent) {
		CurrentOrderComponent->AddOrderInput(InputOrder);
	}
}

ATurnBasePlayerCharacter* UShadowPlayerComponent::GetShadowActor() const
{
	return Cast<ATurnBasePlayerCharacter>(GetChildActor());
}

void UShadowPlayerComponent::ShadowBackspace()
{
	if (bShadowActive) {
		FOrderInput LatestOrder = GetShadowActor()->GetOrderProcessComponent()->GetLatestOrder();
		switch (LatestOrder.OrderType)
		{
		case EMoveOrder:
			GetShadowActor()->SetActorLocation(LatestOrder.CurrentLocation);
			break;
		case EUnknowOrder:
			DestroyChildActor();
			break;
		default:
			break;
		}
	}
}
