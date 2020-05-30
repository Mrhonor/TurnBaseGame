// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBaseCharacter.h"
#include "TurnBaseGameModeBase.h"


// Sets default values
ATurnBaseCharacter::ATurnBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OrderProcessComponent = CreateDefaultSubobject<UOrderProcessComponent>(TEXT("OrderProcessComponent"));

	CurrentGameState = EUnknow;
}

// Called when the game starts or when spawned
void ATurnBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (ATurnBaseGameModeBase* TestGameMode = Cast<ATurnBaseGameModeBase>(GetWorld()->GetAuthGameMode())) {
		TestGameMode->OnGameStateChange.AddDynamic(this, &ATurnBaseCharacter::OnGameStateChangeDelegate);
	}
}

// Called every frame
void ATurnBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATurnBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATurnBaseCharacter::CameraMove(float XValue, float YValue){
}

bool ATurnBaseCharacter::MoveToTargetLocation(bool bForce) {
	FVector CurrentLocation = GetActorLocation();

	if (abs(CurrentLocation.X - GridTargetLocation.X) < 10.f && abs(CurrentLocation.Y - GridTargetLocation.Y) < 10.f) return true;
	
	// find out which way is forward
	const FRotator Rotation = (GridTargetLocation - CurrentLocation).Rotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, 1.0f, bForce);
	return false;
}

void ATurnBaseCharacter::OrderBackspace()
{
}

void ATurnBaseCharacter::OnGameStateChangeDelegate(ETurnBasePlayState NewState)
{
	if (CurrentGameState == NewState) return;
	CurrentGameState = NewState;
}
