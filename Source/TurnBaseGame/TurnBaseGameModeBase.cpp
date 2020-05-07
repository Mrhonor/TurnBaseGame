// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "TurnBaseGameModeBase.h"
#include "Scene/GridScene.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TurnBasePlayerCharacter.h"
#include "Scene/GridPropertyInterface.h"
#include "EngineUtils.h"

const static int CHARACTER_HALF_HEIGHT_ABOVE_GROUND = 88.f;

ATurnBaseGameModeBase::ATurnBaseGameModeBase() {
	SetCurrentGameState(ETurnBasePlayState::EUnknow);
}

void ATurnBaseGameModeBase::BeginPlay() {
	Super::BeginPlay();

	SetCurrentGameState(ETurnBasePlayState::EPlaying);

	// only for test
	if (HUDWidgetBattle != NULL) {
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetBattle);
		if (CurrentWidget != nullptr) {
			CurrentWidget->AddToViewport();
		}
	}
}

void ATurnBaseGameModeBase::SetCurrentGameState(ETurnBasePlayState NewState) {
	if (NewState == CurrentGameState) return;

	switch (NewState)
	{
	case EPlaying:
		break;
	case EBattle:
		if (UWorld* World = GetWorld()) {
			if (GridScene != NULL) {
				if (ATurnBasePlayerCharacter* PlayerCharacter = Cast<ATurnBasePlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))) {
					FVector CharacterLocation = PlayerCharacter->GetActorLocation();
					SpawnedGrid = World->SpawnActor<AGridScene>(GridScene, CharacterLocation - FVector(1000.f, 1000.f, CHARACTER_HALF_HEIGHT_ABOVE_GROUND + 10.f), FRotator::ZeroRotator);

					// not good enough. it should be TActorIterator<IGridPropertyInterface>
					for (TActorIterator<AActor> It(GetWorld()); It; ++It) {
						SpawnedGrid->AddObjectIntoGrid(*It);
					}
				}
			}
		}
		break;
	case EBattling:
		break;
	case EBattleWon:
		break;
	case EBattleLose:
		break;
	case EUnknow:
		break;
	default:
		break;
	}

	CurrentGameState = NewState;
}

void ATurnBaseGameModeBase::ShowSelectSection(const FVector &ShowLocation) {
	if (SpawnedGrid != nullptr) {
		SpawnedGrid->ShowSelectSection(ShowLocation);
	}
}

bool ATurnBaseGameModeBase::MoveObjectTo(AActor* MovedObject, const FVector &Location, bool IsCenter, bool IsMove) {
	if (CurrentGameState != ETurnBasePlayState::EBattle || SpawnedGrid == nullptr) return false;
	else
	{
		int32 Row = 0;
		int32 Col = 0;
		SpawnedGrid->GetGridPosition(Location, Row, Col);
		return SpawnedGrid->MoveObjectTo(MovedObject, Row, Col, IsCenter, IsMove);
	}
}

bool ATurnBaseGameModeBase::MoveCharacterTo(ATurnBaseCharacter* MovedCharacter, const FVector& Location) {
	if (CurrentGameState != ETurnBasePlayState::EBattle || SpawnedGrid == nullptr) return false;
	else
	{
		int32 TargetRow = 0;
		int32 TargetCol = 0;
		int32 CurrentRow = 0;
		int32 CurrentCol = 0;

		SpawnedGrid->GetGridPosition(Location, TargetRow, TargetCol);
		SpawnedGrid->GetGridPosition(MovedCharacter->GetActorLocation(), CurrentRow, CurrentCol);
		int32 DeltaRow = TargetRow - CurrentRow;
		int32 DeltaCol = TargetCol - CurrentCol;

		if (abs(DeltaRow) > 1 || abs(DeltaCol) > 1) return false;
		else  return SpawnedGrid->MoveCharacterTo(MovedCharacter, DeltaRow, DeltaCol);
	}
}

bool ATurnBaseGameModeBase::UpdateGrid(ATurnBaseCharacter* MovedCharacter, const FVector& Location) {
	if (CurrentGameState != ETurnBasePlayState::EBattle || SpawnedGrid == nullptr) return false;
	else {
		return SpawnedGrid->UpdateGrid(MovedCharacter, Location);
	}
}