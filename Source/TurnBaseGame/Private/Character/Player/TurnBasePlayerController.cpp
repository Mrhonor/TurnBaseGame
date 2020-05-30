// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasePlayerController.h"
#include "PlayerCameraPawn.h"
#include "GameFramework/Pawn.h"
#include "TurnBasePlayerCharacter.h"
#include "Engine/World.h"
#include "TurnBaseGameModeBase.h"
#include "GridManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

//  the height spawn camera above current location
static const int CAMERA_SPAWN_HEIGHT = 800.f;


ATurnBasePlayerController::ATurnBasePlayerController() {
	bShowMouseCursor = true;

	PlayerInputMessagePtr = nullptr;
	CurrentGameState = EUnknow;

}

void ATurnBasePlayerController::BeginPlay() {
	Super::BeginPlay();
	CurrentGameMode = Cast<ATurnBaseGameModeBase>(GetWorld()->GetAuthGameMode());
	if (CurrentGameMode) {
		CurrentGameMode->OnGameStateChange.AddDynamic(this, &ATurnBasePlayerController::OnGameStateChangeDelegate);
		CurrentGridManager = CurrentGameMode->FindComponentByClass<UGridManagerComponent>();
	}
}

void ATurnBasePlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);
	
	if (UWorld* World = GetWorld()) {
		FVector MouseLocation;
		FVector MouseDirection;
		DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
		FVector MouseXY = MouseLocation - MouseDirection * (MouseLocation.Z / MouseDirection.Z);

		if (CurrentGameState == ETurnBasePlayState::EBattlePrepare && CurrentGridManager) {
			CurrentGridManager->ShowSelectSection(MouseXY);
		}

		if (CurrentGameState != ETurnBasePlayState::EBattling && CurrentGridManager) {
			float MouseX, MouseY;
			GetMousePosition(MouseX, MouseY);
			float ScreenSizeX = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
			float ScreenSizeY = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
			MouseX /= ScreenSizeX;
			MouseY /= ScreenSizeY;
			MouseX = ScreenMoveMouseClampRange(MouseX);
			MouseY = ScreenMoveMouseClampRange(MouseY);

			if (APlayerCameraPawn* TestCamera = Cast<APlayerCameraPawn>(GetPawn())) {
				TestCamera->CameraMove(MouseX, MouseY);
			}
			else if (ATurnBasePlayerCharacter* TestPlayer = Cast<ATurnBasePlayerCharacter>(GetPawn())) {
				TestPlayer->CameraMove(MouseX, MouseY);
			}
		}
	}
}


void ATurnBasePlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	InputComponent->BindAction("ClickMouseLeft", IE_Pressed, this, &ATurnBasePlayerController::OnClickMouseLeft);
	InputComponent->BindAction("ClickMouseLeft", IE_Released, this, &ATurnBasePlayerController::FinishClickMouseLeft);

	InputComponent->BindAction("ClickMouseRight", IE_Pressed, this, &ATurnBasePlayerController::OnClickMouseRight);
	InputComponent->BindAction("ClickMouseRight", IE_Released, this, &ATurnBasePlayerController::FinishClickMouseRight);

	InputComponent->BindAction("CameraOut", IE_Released, this, &ATurnBasePlayerController::CameraOut);

	InputComponent->BindAction("Battle", IE_Released, this, &ATurnBasePlayerController::Battle);
	InputComponent->BindAction("Battling", IE_Released, this, &ATurnBasePlayerController::Battling);
	InputComponent->BindAction("Backspace", IE_Released, this, &ATurnBasePlayerController::Backspace);

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
		if (TestGameMode->GetCurrentGameState() == ETurnBasePlayState::EPlaying) {
			TestGameMode->SetCurrentGameState(ETurnBasePlayState::EBattle);
		}
	}
}

void ATurnBasePlayerController::Battling(){
	if (CurrentGameState == ETurnBasePlayState::EBattlePrepare) {
		CurrentGameMode->SetCurrentGameState(ETurnBasePlayState::EBattling);
		if (ATurnBasePlayerCharacter* TestPlayer = Cast<ATurnBasePlayerCharacter>(GetPawn())) {
			TestPlayer->ResetCamera();
		}
	}
}

void ATurnBasePlayerController::Backspace(){
	if (ATurnBasePlayerCharacter* TestPlayer = Cast<ATurnBasePlayerCharacter>(GetPawn())) {
		TestPlayer->BackspacePressed();
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

float ATurnBasePlayerController::ScreenMoveMouseClampRange(float MouseValue){
	if (MouseValue > 1.f) MouseValue = 1.f;
	if (MouseValue < 0.f)MouseValue = 0.f;

	if (MouseValue < 0.1f)MouseValue = (MouseValue - 0.1f) *10.f;
	else if (MouseValue > 0.9f)MouseValue = (MouseValue - 0.9f)*10.f;
	else MouseValue = 0.f;
	return MouseValue;
}


void ATurnBasePlayerController::OnGameStateChangeDelegate(ETurnBasePlayState NewState)
{
	if (CurrentGameState == NewState) return;
	CurrentGameState = NewState;
}

void ATurnBasePlayerController::ChangeControlPawn(APawn *InPawn) {
	UnPossess();
	Possess(InPawn);

}