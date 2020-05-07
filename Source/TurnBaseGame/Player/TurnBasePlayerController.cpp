// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasePlayerController.h"
#include "PlayerCameraPawn.h"
#include "GameFramework/Pawn.h"
#include "TurnBasePlayerCharacter.h"
#include "Engine/World.h"
#include "TurnBaseGame/TurnBaseGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

//  the height spawn camera above current location
static const int CAMERA_SPAWN_HEIGHT = 800.f;


ATurnBasePlayerController::ATurnBasePlayerController() {
	bShowMouseCursor = true;

	PlayerInputMessagePtr = nullptr;
}

void ATurnBasePlayerController::BeginPlay() {
	Super::BeginPlay();

	if (UWorld* World = GetWorld()) {
		CurrentGameMode = Cast<ATurnBaseGameModeBase>(World->GetAuthGameMode());
	}
}

void ATurnBasePlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	if (UWorld* World = GetWorld()) {
		FVector MouseLocation;
		FVector MouseDirection;
		DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
		FVector MouseXY = MouseLocation - MouseDirection * (MouseLocation.Z / MouseDirection.Z);

		if (CurrentGameMode != nullptr && CurrentGameMode->GetCurrentGameState() == ETurnBasePlayState::EBattle) {
			CurrentGameMode->ShowSelectSection(MouseXY);
		}
	}
}


void ATurnBasePlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	InputComponent->BindAction("ClickMouseLeft", IE_Pressed, this, &ATurnBasePlayerController::OnClickMouseLeft);
	InputComponent->BindAction("ClickMouseLeft", IE_Released, this, &ATurnBasePlayerController::FinishClickMouseLeft);

	InputComponent->BindAction("ClickMouseRight", IE_Pressed, this, &ATurnBasePlayerController::OnClickMouseRight);
	InputComponent->BindAction("ClickMouseRight", IE_Released, this, &ATurnBasePlayerController::FinishClickMouseRight);

	InputComponent->BindAction("CameraOut", IE_Pressed, this, &ATurnBasePlayerController::CameraOut);

	InputComponent->BindAction("Battle", IE_Pressed, this, &ATurnBasePlayerController::Battle);

	InputComponent->BindAxis("MoveX", this, &ATurnBasePlayerController::MoveX);
	InputComponent->BindAxis("MoveY", this, &ATurnBasePlayerController::MoveY);
	InputComponent->BindAxis("Zoom", this, &ATurnBasePlayerController::Zoom);

}

void ATurnBasePlayerController::OnClickMouseLeft() {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->bMouseLeft = true;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, PlayerInputMessagePtr->CursorHit);
	}
}

void ATurnBasePlayerController::OnClickMouseRight() {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->bMouseRight = true;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, PlayerInputMessagePtr->CursorHit);

		DeprojectMousePositionToWorld(PlayerInputMessagePtr->MouseLocation, PlayerInputMessagePtr->MouseDirection);
	}
}

void ATurnBasePlayerController::FinishClickMouseLeft() {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->bMouseLeft = false;

		GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, PlayerInputMessagePtr->CursorHit);
	}
}

void ATurnBasePlayerController::FinishClickMouseRight() {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->bMouseRight = false;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, PlayerInputMessagePtr->CursorHit);
	}
}

void ATurnBasePlayerController::MoveX(float AxisValue) {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->MovementInput.X = AxisValue;
	}
}

void ATurnBasePlayerController::MoveY(float AxisValue) {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->MovementInput.Y = AxisValue;
	}
}

void ATurnBasePlayerController::Zoom(float AxisValue) {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->MouseWheelValue = AxisValue;
	}
}

void ATurnBasePlayerController::CameraOut() {
	if (APlayerCameraPawn* TestCamera = Cast<APlayerCameraPawn>(GetPawn())) return;

	if (UWorld* World = GetWorld()) {
		FVector SpawnCameraLocation = GetPawn()->GetActorLocation() + FVector(0.f, 0.f, CAMERA_SPAWN_HEIGHT);
		APlayerCameraPawn* SpawnCamera = World->SpawnActor<APlayerCameraPawn>(CameraPawn, SpawnCameraLocation, FRotator::ZeroRotator);
		ChangeControlPawn(SpawnCamera);
	}
}

void ATurnBasePlayerController::Battle() {
	if (ATurnBaseGameModeBase* TestGameMode = Cast<ATurnBaseGameModeBase>(GetWorld()->GetAuthGameMode())) {
		TestGameMode->SetCurrentGameState(ETurnBasePlayState::EBattle);
	}
}

void ATurnBasePlayerController::Battling(){
	if (ATurnBaseGameModeBase* TestGameMode = Cast<ATurnBaseGameModeBase>(GetWorld()->GetAuthGameMode())) {
		if (TestGameMode->GetCurrentGameState() == ETurnBasePlayState::EBattle) {
			TestGameMode->SetCurrentGameState(ETurnBasePlayState::EBattling);
		}
	}
}

void ATurnBasePlayerController::OnPossess(APawn *InPawn) {
	Super::OnPossess(InPawn);

	if (APlayerCameraPawn* TestCamera = Cast<APlayerCameraPawn>(InPawn)) {
		PlayerInputMessagePtr = &TestCamera->PlayerInputMessage;
		TestCamera->OnControl(true);
	}
	if (ATurnBasePlayerCharacter* TestPlayer = Cast<ATurnBasePlayerCharacter>(InPawn)) {
		PlayerInputMessagePtr = &TestPlayer->PlayerInputMessage;
		TestPlayer->OnControl(true);
	}
}

void ATurnBasePlayerController::OnUnPossess() {

	PlayerInputMessagePtr = nullptr;
	if (APlayerCameraPawn* TestCamera = Cast<APlayerCameraPawn>(GetPawn())) {
		TestCamera->ExitControl();
	}
	if (ATurnBasePlayerCharacter* TestPlayer = Cast<ATurnBasePlayerCharacter>(GetPawn())) {
		TestPlayer->ExitControl();
	}

	Super::OnUnPossess();
}


void ATurnBasePlayerController::ChangeControlPawn(APawn *InPawn) {
	UnPossess();
	Possess(InPawn);

}