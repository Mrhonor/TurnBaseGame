// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "TurnBaseGameModeBase.h"
#include "Scene/GridScene.h"
#include "Blueprint/UserWidget.h"
#include "GridPropertyInterface.h"
#include "GridManagerComponent.h"



ATurnBaseGameModeBase::ATurnBaseGameModeBase() {
	GridManagerComponent = CreateDefaultSubobject<UGridManagerComponent>(TEXT("GridManagerComponent"));

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
	CurrentGameState = NewState;
	OnGameStateChange.Broadcast(CurrentGameState);
	switch (NewState)
	{
	case ETurnBasePlayState::EPlaying:
		break;
	case ETurnBasePlayState::EBattle:
		if (GridManagerComponent && GridManagerComponent->GridSpawn())
		{
			SetCurrentGameState(ETurnBasePlayState::EBattlePrepare);
		}
		break;
	case ETurnBasePlayState::EBattlePrepare:
		if (GridManagerComponent->GetSpawnedGrid()) {
			GridManagerComponent->GetSpawnedGrid()->bOrderProcess = false;
			GridManagerComponent->GetSpawnedGrid()->EnableCharacterBattleInput(true);
		}
		break;
	case ETurnBasePlayState::EBattling:
		if (GridManagerComponent->GetSpawnedGrid()) {
			GridManagerComponent->GetSpawnedGrid()->EnableCharacterBattleInput(false);
			GridManagerComponent->GetSpawnedGrid()->bOrderProcess = true;
			GridManagerComponent->GetSpawnedGrid()->ClearAllShadowCharacter();
		}
		break;
	case ETurnBasePlayState::EBattleWon:
		break;
	case ETurnBasePlayState::EBattleLose:
		break;
	case ETurnBasePlayState::EUnknow:
		break;
	default:
		break;
	}

}