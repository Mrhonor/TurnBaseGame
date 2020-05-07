// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraPawn.h"
#include "Components/InputComponent.h"
#include "TurnBaseGame/Character/TurnBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"




// Sets default values
APlayerCameraPawn::APlayerCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (RootComponent == nullptr) {
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	MainCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	CameraMoveSpeed = 800.f;
	bControlled = false;
	StartHit = false;
}

// Called when the game starts or when spawned
void APlayerCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (bControlled) {
		FVector2D MoveXY = PlayerInputMessage.MovementInput.GetSafeNormal() * CameraMoveSpeed * DeltaTime + FVector2D(GetActorLocation().X, GetActorLocation().Y);
		FVector Movement = FVector(MoveXY, GetActorLocation().Z);
		SetActorLocation(Movement);

		MouseWheelRoll();

		if (PlayerInputMessage.bMouseLeft && !StartHit) {
			StartHit = true;
			CursorHitActor = PlayerInputMessage.CursorHit.GetActor();
		}
		else if (!PlayerInputMessage.bMouseLeft && StartHit) {
			StartHit = false;
			if (CursorHitActor == PlayerInputMessage.CursorHit.GetActor()) {
				ChangeControl();
			}
		}
	}
}


bool APlayerCameraPawn::MouseLeftKeyProcess(const FHitResult& Hit) {
	if (Hit.GetActor() == nullptr) return false;


	return true;
}

void APlayerCameraPawn::ResetPlayerControl() {
	bControlled = false;
	PlayerInputMessage.Empty();
}

void APlayerCameraPawn::MouseWheelRoll() {
	if (PlayerInputMessage.MouseWheelValue != 0.f) {
		SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, PlayerInputMessage.MouseWheelValue));
	}
}

void APlayerCameraPawn::ChangeControl() {
	if (ATurnBaseCharacter* TestCharacter = Cast<ATurnBaseCharacter>(CursorHitActor)) {
		if (ATurnBasePlayerController* TestController = Cast<ATurnBasePlayerController>(Controller)) {
			ResetPlayerControl();
			TestController->ChangeControlPawn(TestCharacter);
			Destroy();
		}
	}
}

void APlayerCameraPawn::ExitControl() {
	ResetPlayerControl();
	Destroy();
}