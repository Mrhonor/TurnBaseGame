// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasePlayerController.h"
#include "PlayerCameraPawn.h"
#include "GameFramework/Pawn.h"
#include "TurnBasePlayerCharacter.h"

ATurnBasePlayerController::ATurnBasePlayerController() {
	bShowMouseCursor = true;

	PlayerInputMessagePtr = nullptr;
}

void ATurnBasePlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

}


void ATurnBasePlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	InputComponent->BindAction("ClickMouseLeft", IE_Pressed, this, &ATurnBasePlayerController::OnClickMouseLeft);
	InputComponent->BindAction("ClickMouseLeft", IE_Released, this, &ATurnBasePlayerController::FinishClickMouseLeft);

	InputComponent->BindAction("ClickMouseRight", IE_Pressed, this, &ATurnBasePlayerController::OnClickMouseRight);
	InputComponent->BindAction("ClickMouseRight", IE_Released, this, &ATurnBasePlayerController::FinishClickMouseRight);

	InputComponent->BindAxis("MoveX", this, &ATurnBasePlayerController::MoveX);
	InputComponent->BindAxis("MoveY", this, &ATurnBasePlayerController::MoveY);
	InputComponent->BindAxis("Zoom", this, &ATurnBasePlayerController::Zoom);

}

void ATurnBasePlayerController::OnClickMouseLeft() {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->bMouseLeft = true;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, PlayerInputMessagePtr->CursorHit);
	}
}

void ATurnBasePlayerController::OnClickMouseRight() {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->bMouseRight = true;
	}
}

void ATurnBasePlayerController::FinishClickMouseLeft() {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->bMouseLeft = false;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, PlayerInputMessagePtr->CursorHit);
	}
}

void ATurnBasePlayerController::FinishClickMouseRight() {
	if (PlayerInputMessagePtr != nullptr) {
		PlayerInputMessagePtr->bMouseRight = false;
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
		PlayerInputMessagePtr->MovementInput.Y = AxisValue;
	}
}

void ATurnBasePlayerController::OnPossess(APawn *InPawn) {
	Super::OnPossess(InPawn);

	if (APlayerCameraPawn* TestCamera = Cast<APlayerCameraPawn>(InPawn)) {
		PlayerInputMessagePtr = &TestCamera->PlayerInputMessage;
		TestCamera->OnControl(true);
	}
	else if (ATurnBasePlayerCharacter* TestPlayer = Cast<ATurnBasePlayerCharacter>(InPawn)) {
		PlayerInputMessagePtr = &TestPlayer->PlayerInputMessage;
		TestPlayer->OnControl(true);
	}
}

void ATurnBasePlayerController::OnUnPossess() {

	PlayerInputMessagePtr = nullptr;
	if (APlayerCameraPawn* TestCamera = Cast<APlayerCameraPawn>(GetPawn())) {
		TestCamera->OnControl(false);
	}
	else if (ATurnBasePlayerCharacter* TestPlayer = Cast<ATurnBasePlayerCharacter>(GetPawn())) {
		TestPlayer->OnControl(false);
	}

	Super::OnUnPossess();
}


void ATurnBasePlayerController::ChangeControlPawn(APawn *InPawn) {
	UnPossess();
	Possess(InPawn);

}