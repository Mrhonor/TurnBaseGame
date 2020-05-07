// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TurnBaseGame/TurnBaseGameModeBase.h"
#include "Camera/CameraComponent.h"


ATurnBasePlayerCharacter::ATurnBasePlayerCharacter() {
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;


	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bControlled = false;

	PlayerInputMessage.Empty();
	PrimaryActorTick.bCanEverTick = true;

	IsNeedToMove = false;
}

void ATurnBasePlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	GridTargetLocation = GetActorLocation();
}

void ATurnBasePlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (bControlled) {
		MouseWheelRoll();

		if (PlayerInputMessage.bMouseRight == true) {
			ConsumeMouseRightInput();
		}

		if (IsNeedToMove) {
			if (MoveToTargetLocation()) {
				IsNeedToMove = false;
				if (ATurnBaseGameModeBase* TestGameMode = Cast<ATurnBaseGameModeBase>(GetWorld()->GetAuthGameMode())) {
					TestGameMode->UpdateGrid(this, PreGridLocation);
				}
			}
		}
	}

}

void ATurnBasePlayerCharacter::MouseWheelRoll() {
	if (PlayerInputMessage.MouseWheelValue != 0.f) {
		CameraBoom->AddRelativeLocation(FVector(0.f, 0.f, PlayerInputMessage.MouseWheelValue));
	}
}

void ATurnBasePlayerCharacter::ConsumeMouseRightInput() {
	PlayerInputMessage.bMouseRight = false;
	if (ATurnBaseGameModeBase* TestGameMode = Cast<ATurnBaseGameModeBase>(GetWorld()->GetAuthGameMode())) {
		FVector MouseXY = PlayerInputMessage.MouseLocation - PlayerInputMessage.MouseDirection * (PlayerInputMessage.MouseLocation.Z / PlayerInputMessage.MouseDirection.Z);
		if (TestGameMode->MoveCharacterTo(this, MouseXY)) {
			IsNeedToMove = true;
			PreGridLocation = GetActorLocation();
		}
	}
}

void ATurnBasePlayerCharacter::ExitControl() {
	PlayerInputMessage.Empty();
	OnControl(false);
}

