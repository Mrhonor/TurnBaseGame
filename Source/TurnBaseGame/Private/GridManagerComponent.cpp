// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManagerComponent.h"
#include "TurnBasePlayerCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TurnBaseGameModeBase.h"
#include "GridScene.h"
#include "EngineUtils.h"


const static int CHARACTER_HALF_HEIGHT_ABOVE_GROUND = 88.f;

// Sets default values for this component's properties
UGridManagerComponent::UGridManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	CurrentGameState = ETurnBasePlayState::EUnknow;
	// ...
}


bool UGridManagerComponent::GridSpawn()
{
	if (UWorld* World = GetWorld()) {
		if (GridScene != NULL) {
			if (ATurnBasePlayerCharacter* PlayerCharacter = Cast<ATurnBasePlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))) {
				FVector CharacterLocation = PlayerCharacter->GetActorLocation();
				SpawnedGrid = World->SpawnActor<AGridScene>(GridScene, CharacterLocation - FVector(1000.f, 1000.f, CHARACTER_HALF_HEIGHT_ABOVE_GROUND + 10.f), FRotator::ZeroRotator);

				// not good enough. it should be TActorIterator<IGridPropertyInterface>
				for (TActorIterator<AActor> It(GetWorld()); It; ++It) {
					SpawnedGrid->AddObjectIntoGrid(*It);
				}
				return true;
			}
		}
	}
	return false;
}

// Called when the game starts
void UGridManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (ATurnBaseGameModeBase* TestGameMode = Cast<ATurnBaseGameModeBase>(GetWorld()->GetAuthGameMode())) {
		TestGameMode->OnGameStateChange.AddDynamic(this, &UGridManagerComponent::OnGameStateChangeDelegate);
	}
}

void UGridManagerComponent::OnGameStateChangeDelegate(ETurnBasePlayState NewState)
{
	if (CurrentGameState == NewState) return;
	CurrentGameState = NewState;

}

void UGridManagerComponent::ShowSelectSection(const FVector &ShowLocation) {
	if (SpawnedGrid != nullptr) {
		SpawnedGrid->ShowSelectSection(ShowLocation);
	}
}

bool UGridManagerComponent::OrderValidity(ATurnBaseCharacter* Character, const FOrderInput & Order) {
	if (CurrentGameState == EBattlePrepare) {
		if (SpawnedGrid != nullptr) {
			switch (Order.OrderType)
			{
			case EOrderType::EMoveOrder:
				if (SpawnedGrid != nullptr) {
					return SpawnedGrid->CanCharacterMoveTo(Character->GetActorLocation(), Order.TargetLocation);
				}
				return false;
			default:
				return true;
			}
		}
	}

	return false;
}

FVector UGridManagerComponent::GetGridCenter(const FVector & Location)
{
	if (SpawnedGrid != nullptr) {
		return SpawnedGrid->GetGridCenter(Location);
	}
	else {
		return Location;
	}
}

bool UGridManagerComponent::MoveObjectTo(AActor* MovedObject, const FVector &Location, bool IsCenter, bool IsMove) {
	if (SpawnedGrid == nullptr) return false;
	else
	{
		int32 Row = 0;
		int32 Col = 0;
		SpawnedGrid->GetGridPosition(Location, Row, Col);
		return SpawnedGrid->MoveObjectTo(MovedObject, Row, Col, IsCenter, IsMove);
	}
}

bool UGridManagerComponent::MoveCharacterTo(ATurnBaseCharacter* MovedCharacter, const FVector& Location) {
	if (SpawnedGrid == nullptr) return false;
	else
	{
		if (SpawnedGrid->CanCharacterMoveTo(MovedCharacter->GetActorLocation(), Location)) {
			return SpawnedGrid->MoveCharacterToLocation(MovedCharacter, Location);
		}
		return false;
	}
}

bool UGridManagerComponent::UpdateGrid(ATurnBaseCharacter* MovedCharacter, const FVector& Location) {
	if (SpawnedGrid == nullptr) return false;
	else {
		return SpawnedGrid->UpdateGrid(MovedCharacter, Location);
	}
}


FOrderInput UGridManagerComponent::GetLatestOrder(ATurnBaseCharacter * TheCharacter) {
	if (SpawnedGrid) {
		return SpawnedGrid->GetLatestOrder(TheCharacter);
	}
	return FOrderInput();
}

void UGridManagerComponent::ClearLatestOrder(ATurnBaseCharacter* TheCharacter)
{
	if (SpawnedGrid) {
		SpawnedGrid->ClearLatestOrder(TheCharacter);
	}
}
