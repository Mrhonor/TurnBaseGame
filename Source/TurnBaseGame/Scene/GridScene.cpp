// Fill out your copyright notice in the Description page of Project Settings.


#include "GridScene.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AGridScene::AGridScene()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AGridScene::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGridScene::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

