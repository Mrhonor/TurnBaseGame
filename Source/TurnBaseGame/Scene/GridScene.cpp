// Fill out your copyright notice in the Description page of Project Settings.


#include "GridScene.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AGridScene::AGridScene()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));

	LineProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("LineProceduralMesh"));
	LineProceduralMesh->SetupAttachment(RootComponent);

	TileProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TileProceduralMesh"));
	TileProceduralMesh->SetupAttachment(RootComponent);

	LineOpacity = 1.f;
	LineThickness = 5.f;
	TileOpacity = 0.3f;
	TileRow = 10;
	TileCol = 10;
	TileSize = 100.f;

}

// Called when the game starts or when spawned
void AGridScene::BeginPlay()
{
	Super::BeginPlay();
}

void AGridScene::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	//this->UnregisterAllComponents();
	if (LineMaterial != nullptr) {
		UMaterialInstanceDynamic* LineMaterialInstance = CreateMaterialInstance(LineMaterial, LineColor, LineOpacity);

		TArray<FVector> Verticles;
		TArray<int32>   Triangles;
		CreateLine(Verticles, Triangles);

		TArray<FVector> Normals;
		TArray<FVector2D> UV0;
		TArray<FColor> VertexColors;
		TArray<FProcMeshTangent> Tangents;
		bool bCreateCollision = false;
		LineProceduralMesh->CreateMeshSection(0, Verticles, Triangles, Normals, UV0, VertexColors, Tangents, bCreateCollision);
		LineProceduralMesh->SetMaterial(0, LineMaterialInstance);
	}

	if (TileMaterial != nullptr) {
		UMaterialInstanceDynamic* TileMaterialInstance = CreateMaterialInstance(TileMaterial, TileColor, TileOpacity);
		TArray<FVector> Verticles;
		TArray<int32>   Triangles;
		CreateTile(Verticles, Triangles);

		TArray<FVector> Normals;
		TArray<FVector2D> UV0;
		TArray<FColor> VertexColors;
		TArray<FProcMeshTangent> Tangents;
		bool bCreateCollision = false;
		TileProceduralMesh->CreateMeshSection(0, Verticles, Triangles, Normals, UV0, VertexColors, Tangents, bCreateCollision);
		TileProceduralMesh->SetVisibility(false);
		TileProceduralMesh->SetMaterial(0, TileMaterialInstance);
	}

	//this->RegisterAllComponents();
}

bool AGridScene::IsInRange(int32 Row, int32 Col) {
	return (Row >= 0 && Row < TileRow) && (Col >= 0 && Col < TileRow);
}

bool AGridScene::GetGridPosition(const FVector& Location, int32 &Row, int32 &Col) {
	FVector RelativeLocation = Location - GetActorLocation();
	Col = int(RelativeLocation.X / TileSize);
	Row = int(RelativeLocation.Y / TileSize);

	return IsInRange(Row, Col);
}

UMaterialInstanceDynamic* AGridScene::CreateMaterialInstance(UMaterialInterface *SourceMaterial,const FLinearColor &Color, float Opacity) {
	UMaterialInstanceDynamic * Instance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(0, SourceMaterial);
	Instance->SetVectorParameterValue(FName("Color"), Color);
	Instance->SetScalarParameterValue(FName("Opacity"), Opacity);
	return Instance;
}

UMaterialInstanceDynamic* AGridScene::CreateDefaultMaterialInstance(UMaterialInterface *SourceMaterial) {
	return UKismetMaterialLibrary::CreateDynamicMaterialInstance(0, SourceMaterial);
}

void AGridScene::CreateLine(TArray<FVector> &Vertices, TArray<int32>& Triangles) {
	float GridWidth = GetGridWidth();
	float GridHeight = GetGridHeight();
	float HalfThickness = LineThickness / 2.f;

	// using lambda
	auto AppendTrianglesArray = [&Vertices, &Triangles](){
		int32 VerticeLength = Vertices.Num(); 
		Triangles.Add(VerticeLength + 2);
		Triangles.Add(VerticeLength + 1);
		Triangles.Add(VerticeLength + 0);
		Triangles.Add(VerticeLength + 2);
		Triangles.Add(VerticeLength + 3);
		Triangles.Add(VerticeLength + 1); 
	};

	auto AppendVerticesArray = [&Vertices, HalfThickness](const FVector& Start, const FVector& End, const FVector& ThicknessDirection) {
		Vertices.Add(Start + HalfThickness * ThicknessDirection);
		Vertices.Add(End + HalfThickness * ThicknessDirection);
		Vertices.Add(Start - HalfThickness * ThicknessDirection);
		Vertices.Add(End - HalfThickness * ThicknessDirection);
	};

	// draw line for row
	for (int i = 0; i <= TileRow; i++) {
		FVector LineStart(0.f, i * TileSize, 0.f);
		FVector LineEnd(GridWidth, i * TileSize, 0.f);
		FVector ThicknessDirection = (LineEnd - LineStart).GetSafeNormal();
		ThicknessDirection = FVector::CrossProduct(ThicknessDirection, FVector(0.f, 0.f, 1.f));

		// make triangles array
		AppendTrianglesArray();

		// make vertices array
		AppendVerticesArray(LineStart, LineEnd, ThicknessDirection);
	}

	// draw line for col
	for (int i = 0; i <= TileCol; i++) {
		FVector LineStart(i * TileSize, 0.f, 0.f);
		FVector LineEnd(i * TileSize, GridHeight, 0.f);
		FVector ThicknessDirection = (LineEnd - LineStart).GetSafeNormal();
		ThicknessDirection = FVector::CrossProduct(ThicknessDirection, FVector(0.f, 0.f, 1.f));
		
		// make triangles array
		AppendTrianglesArray();

		// make vertices array
		AppendVerticesArray(LineStart, LineEnd, ThicknessDirection);
	}
}

void AGridScene::CreateTile(TArray<FVector> &Vertices, TArray<int32>& Triangles) {
	float HalfThickness = TileSize / 2.f;
	// draw line for row
	FVector Start(0.f, TileSize / 2.f, 0.f);
	FVector End(TileSize, TileSize / 2.f, 0.f);
	FVector ThicknessDirection = (End - Start).GetSafeNormal();
	ThicknessDirection = FVector::CrossProduct(ThicknessDirection, FVector(0.f, 0.f, 1.f));

	int32 VerticeLength = Vertices.Num();
	Triangles.Add(VerticeLength + 2);
	Triangles.Add(VerticeLength + 1);
	Triangles.Add(VerticeLength + 0);
	Triangles.Add(VerticeLength + 2);
	Triangles.Add(VerticeLength + 3);
	Triangles.Add(VerticeLength + 1);

	Vertices.Add(Start + HalfThickness * ThicknessDirection);
	Vertices.Add(End + HalfThickness * ThicknessDirection);
	Vertices.Add(Start - HalfThickness * ThicknessDirection);
	Vertices.Add(End - HalfThickness * ThicknessDirection);
}
