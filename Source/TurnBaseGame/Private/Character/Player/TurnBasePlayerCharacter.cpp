// Fill out your copyright notice in the Description page of Project Settings.

#include "TurnBasePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "OrderInput.h"
#include "GridManagerComponent.h"
#include "ShadowPlayerComponent.h"
#include "GameFramework/GameModeBase.h"



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

	ShadowPlayerComponent = CreateDefaultSubobject<UShadowPlayerComponent>(TEXT("ShadowCharacter"));
	ShadowPlayerComponent->SetupAttachment(GetMesh());

	bControlled = false;

	PlayerInputMessage.Empty();
	PrimaryActorTick.bCanEverTick = true;

	bNeedToMove = false;
}


void ATurnBasePlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	GridTargetLocation = GetActorLocation();
	CameraTransform = CameraBoom->GetRelativeTransform();

	CurrentGridManager = GetWorld()->GetAuthGameMode()->FindComponentByClass<UGridManagerComponent>();
}

void ATurnBasePlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	
	if (bControlled) {
		MouseWheelRoll();

		if (PlayerInputMessage.bMouseRight == true) {
			ConsumeMouseRightInput();
		}

	}


	// Execute Shadow Character mission
	if (bNeedToMove) {
		if (MoveToTargetLocation(true)) {
			bNeedToMove = false;
			OrderProcessComponent->SetOrderExecuting(false);
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
	FVector MouseXY = PlayerInputMessage.MouseLocation - PlayerInputMessage.MouseDirection * (PlayerInputMessage.MouseLocation.Z / PlayerInputMessage.MouseDirection.Z);
	switch (CurrentGameState)
	{
	case EBattlePrepare:
		if (!OrderProcessComponent->CanAddOrderInput()) return;
		
		if (ShadowPlayerComponent != nullptr) {
			if (!ShadowPlayerComponent->GetShadowActive()) {
				ShadowPlayerComponent->SetShadowActive(true);
			}

			if (ATurnBasePlayerCharacter* Shadow = ShadowPlayerComponent->GetShadowActor()) {
				if (Shadow->GetOrderExecuting() == false) {
					FOrderInput Order(EOrderType::EMoveOrder, MouseXY, Shadow->GetActorLocation());
					if (CurrentGridManager->OrderValidity(Shadow, Order)) {
						OrderProcessComponent->AddOrderInput(Order);
						Shadow->GetOrderProcessComponent()->AddOrderInput(Order);
						Shadow->GetOrderProcessComponent()->ExecutrFirstOrder();
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

void ATurnBasePlayerCharacter::OrderBackspace(){
	if (CurrentGameState == ETurnBasePlayState::EBattlePrepare) {
		CurrentGridManager->ClearLatestOrder(this);
	}
}

void ATurnBasePlayerCharacter::ExitControl() {
	PlayerInputMessage.Empty();
	OnControl(false);
}

void ATurnBasePlayerCharacter::ResetCamera(){
	CameraBoom->SetRelativeTransform(CameraTransform);
}

void ATurnBasePlayerCharacter::DestroyShadowCharacter(){
	if (ShadowPlayerComponent) {
		ShadowPlayerComponent->SetShadowActive(false);
	}
}

void ATurnBasePlayerCharacter::AddGridMovementInput(const FVector & TargetLocation){
	PreGridLocation = GetActorLocation();
	GridTargetLocation = TargetLocation;
	bNeedToMove = true;
}

void ATurnBasePlayerCharacter::CameraMove(float XValue, float YValue){
	const float Scale = 20.f;
	CameraBoom->SetWorldLocation(CameraBoom->GetComponentLocation() + FVector(-YValue * Scale, XValue * Scale, 0));
}

void ATurnBasePlayerCharacter::BackspacePressed(){
	if (ShadowPlayerComponent) {
		ShadowPlayerComponent->ShadowBackspace();
	}
	OrderBackspace();
}


