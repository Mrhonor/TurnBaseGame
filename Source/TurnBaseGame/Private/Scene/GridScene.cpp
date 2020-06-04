// Fill out your copyright notice in the Description page of Project Settings.


#include "GridScene.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "GridPropertyInterface.h"
#include "TurnBaseCharacter.h"
#include "TurnBasePlayerCharacter.h"
#include "Engine/World.h"
#include "TurnBaseGameModeBase.h"
#include "OrderProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

const static int CHARACTER_HALF_HEIGHT_ABOVE_GROUND = 88.f;

FStorageObjectList::FStorageObjectList() {
	Row = -1;
	Col = -1;
	StoragedObject = nullptr;
	NextObjectInRow = nullptr;
	NextObjectInCol = nullptr;
	OverlapObject = nullptr;
}

FStorageObjectList::FStorageObjectList(int32 row, int32 col, IGridPropertyInterface* storagedObject, FStorageObjectList* nextObjectInRow, 
	FStorageObjectList* nextObjectInCol, FStorageObjectList* overlapObject) {
	Row = row;
	Col = col;
	StoragedObject = storagedObject;
	NextObjectInRow = nextObjectInRow;
	NextObjectInCol = nextObjectInCol;
	OverlapObject = overlapObject;
}

FStorageCharacter::FStorageCharacter()
	: Row(-1),
	  Col(-1),
	  TheCharacter(nullptr),
	  OrderProcessComponent(nullptr),
	  IsPlayerCharacter(false)
{
}

FStorageCharacter::FStorageCharacter(int32 row, int32 col, ACharacter* theCharacter, bool isPlayerCharacter)
	: Row(row),
	Col(col),
	TheCharacter(theCharacter),
	IsPlayerCharacter(isPlayerCharacter)
{
	if (theCharacter) {
		OrderProcessComponent = theCharacter->FindComponentByClass<UOrderProcessComponent>();
	}
}

bool operator==(const FStorageCharacter &A, const FStorageCharacter &B) {
	return (
		A.Row == B.Row &&
		A.Col == B.Col &&
		A.TheCharacter == B.TheCharacter &&
		A.IsPlayerCharacter == B.IsPlayerCharacter 
		);
}

// Sets default values
AGridScene::AGridScene()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	ObjectTotalNum = 0;
	ObjectRowNum = 0;
	ObjectColNum = 0;
	bOrderProcess = false;
}

AGridScene::~AGridScene() {
	for (FStorageObjectList* TheObjectNeedDeleted : ObjectColList) {
		DeleteTheColList(TheObjectNeedDeleted);
	}
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

	ObjectRowList.Init(nullptr, TileRow);
	ObjectColList.Init(nullptr, TileCol);

	//this->RegisterAllComponents();
}

void AGridScene::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	if (bOrderProcess) {
		if (!OrderProcess()) {
			if (ATurnBaseGameModeBase* TestGameMode = Cast<ATurnBaseGameModeBase>(GetWorld()->GetAuthGameMode())) {
				TestGameMode->SetCurrentGameState(ETurnBasePlayState::EBattlePrepare);
			}
		}
	}
}

bool AGridScene::IsInRange(int32 Row, int32 Col) {
	return (Row >= 0 && Row < TileRow) && (Col >= 0 && Col < TileRow);
}

bool AGridScene::GetGridPosition(const FVector& Location, int32 &Row, int32 &Col) {
	FVector RelativeLocation = Location - GetActorLocation();
	Col = int(RelativeLocation.Y / TileSize);
	Row = int(RelativeLocation.X / TileSize);

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

bool AGridScene::AddObjectIntoGrid(AActor* NewGridObject) {
	if (IGridPropertyInterface* TestInterface = Cast<IGridPropertyInterface>(NewGridObject)) {
		int32 GridRow = 0;
		int32 GridCol = 0;
		if (GetGridPosition(NewGridObject->GetActorLocation(), GridRow, GridCol)) {
			if (InsertObjectIntoCrossList(TestInterface, GridRow, GridCol)) {

				if (ATurnBaseCharacter *TestCharacter = Cast<ATurnBaseCharacter>(NewGridObject)) {
					TestCharacter->SetActorLocation(GetGridLocation(GridRow, GridCol) + FVector(0.f, 0.f, CHARACTER_HALF_HEIGHT_ABOVE_GROUND));
					TestCharacter->AddGridMovementInput(GetGridLocation(GridRow, GridCol));
					CharacterArray.AddUnique(FStorageCharacter(GridRow, GridCol, TestCharacter, false));
				}
				return true;
			}
		}
	}
	return false;
}

bool AGridScene::IsBlockingObject(int32 Row, int32 Col) {
	FStorageObjectList* AllObject = GetGridObject(Row, Col);
	while (AllObject != nullptr) {
		if (AllObject->StoragedObject->IsBlockTheCharacter()) {
			return	true;
		}
		AllObject = AllObject->OverlapObject;
	}

	return false;
}

FStorageObjectList* AGridScene::GetGridObject(int32 Row, int32 Col) {
	if (Row <= Col) {
		FStorageObjectList* SearchPtr = ObjectColList[Col];
		if (SearchPtr == nullptr) return nullptr;

		while (SearchPtr != nullptr && SearchPtr->Row < Row) {
			SearchPtr = SearchPtr->NextObjectInCol;
		}
		if (SearchPtr == nullptr || SearchPtr->Row != Row) return nullptr;
		else					   return SearchPtr;
	}
	else {
		FStorageObjectList* SearchPtr = ObjectRowList[Row];
		if (SearchPtr == nullptr) return nullptr;

		while (SearchPtr != nullptr && SearchPtr->Col < Col) {
			SearchPtr = SearchPtr->NextObjectInRow;
		}
		if (SearchPtr == nullptr || SearchPtr->Col != Col) return nullptr;
		else					   return SearchPtr;
	}
}

void AGridScene::ClearAllShadowCharacter(){
	for (FStorageCharacter &i : CharacterArray) {
		if (ATurnBasePlayerCharacter* TestCharacter = Cast<ATurnBasePlayerCharacter>(i.TheCharacter)) {
			TestCharacter->DestroyShadowCharacter();
		}
	}
}

FVector AGridScene::GetGridLocation(int32 Row, int32 Col, bool IsCenter) {
	FVector TargetLocation = GetActorLocation() + FVector(Row * TileSize, Col * TileSize, 0.f);
	if (IsCenter)	return TargetLocation + FVector(TileSize / 2.f, TileSize / 2.f, 0.f);
	else			return TargetLocation;
}

FVector AGridScene::GetGridCenter(const FVector & Location)
{
	int32 Row, Col;
	GetGridPosition(Location, Row, Col);
	return GetGridLocation(Row, Col, true);
}

void AGridScene::ShowSelectSection(const FVector &ShowLocation) {
	int32 Row = 0;
	int32 Col = 0;
	GetGridPosition(ShowLocation, Row, Col);
	if (IsInRange(Row, Col)) {
		TileProceduralMesh->SetWorldLocation(GetGridLocation(Row, Col, false));
		TileProceduralMesh->SetVisibility(true);
	}
}

bool AGridScene::MoveObjectTo(AActor* MovedObject, int32 Row, int32 Col, bool IsCenter, bool IsMove) {
	IGridPropertyInterface* TestInterface = Cast<IGridPropertyInterface>(MovedObject);
	
	if (TestInterface == nullptr || !IsInRange(Row, Col)) return false;

	int32 CurrentRow = 0;
	int32 CurrentCol = 0;

	GetGridPosition(MovedObject->GetActorLocation(), CurrentRow, CurrentCol);
	if (CurrentRow == Row && CurrentCol == Col) return true;
	if (IsInRange(CurrentRow, CurrentCol)) {
		FStorageObjectList* ObjectList = GetGridObject(CurrentRow, CurrentCol);

		if (ObjectList == nullptr) {
			return InsertObjectIntoCrossList(TestInterface, Row, Col);
		}

		if (ObjectList->OverlapObject == nullptr) {
			DeleteStorageObject(CurrentRow, CurrentCol);
		}

		while (ObjectList->OverlapObject != nullptr) {
			if (ObjectList->OverlapObject->StoragedObject == TestInterface) {
				FStorageObjectList* TargetPtr = ObjectList->OverlapObject;
				ObjectList->OverlapObject = TargetPtr->OverlapObject;
				
				delete TargetPtr;
				break;
			}
		}
	}

	if (InsertObjectIntoCrossList(TestInterface, Row, Col)) {
		if(IsMove)
			MovedObject->SetActorLocation(GetGridLocation(Row, Col, IsCenter) + FVector(0.f, 0.f, CHARACTER_HALF_HEIGHT_ABOVE_GROUND));

		return true;
	}
	else return false;

}

bool AGridScene::MoveObjectTo(AActor * MovedObject, int32 PreRow, int32 PreCol, int32 Row, int32 Col, bool IsCenter, bool IsMove)
{
	IGridPropertyInterface* TestInterface = Cast<IGridPropertyInterface>(MovedObject);

	if (TestInterface == nullptr || !IsInRange(Row, Col)) return false;

	if (PreRow == Row && PreCol == Col) return true;
	if (IsInRange(PreRow, PreCol)) {
		FStorageObjectList* ObjectList = GetGridObject(PreRow, PreCol);

		if (ObjectList == nullptr) {
			return InsertObjectIntoCrossList(TestInterface, Row, Col);
		}

		if (ObjectList->OverlapObject == nullptr) {
			DeleteStorageObject(PreRow, PreCol);
		}

		while (ObjectList->OverlapObject != nullptr) {
			if (ObjectList->OverlapObject->StoragedObject == TestInterface) {
				FStorageObjectList* TargetPtr = ObjectList->OverlapObject;
				ObjectList->OverlapObject = TargetPtr->OverlapObject;

				delete TargetPtr;
				break;
			}
		}
	}

	if (InsertObjectIntoCrossList(TestInterface, Row, Col)) {
		if (IsMove)
			MovedObject->SetActorLocation(GetGridLocation(Row, Col, IsCenter) + FVector(0.f, 0.f, CHARACTER_HALF_HEIGHT_ABOVE_GROUND));

		return true;
	}
	else return false;
}

bool AGridScene::MoveCharacterByDeltaIndex(ATurnBaseCharacter* MovedCharacter, int32 DeltaRow, int32 DeltaCol) {
	if (DeltaRow == 0 && DeltaCol == 0) return true;

	int32 CurrentRow = 0;
	int32 CurrentCol = 0;

	GetGridPosition(MovedCharacter->GetActorLocation(), CurrentRow, CurrentCol);

	int32 TargetRow = CurrentRow + DeltaRow;
	int32 TargetCol = CurrentCol + DeltaCol;

	if (!IsInRange(TargetRow, TargetRow) || IsBlockingObject(TargetRow, TargetCol)) return false;

	MovedCharacter->AddGridMovementInput(GetGridLocation(TargetRow, TargetCol));

	return true;
}

bool AGridScene::MoveCharacterToLocation(ATurnBaseCharacter * MovedCharacter, const FVector & TargetLocation)
{
	int32 TargetRow = 0;
	int32 TargetCol = 0;
	if (GetGridPosition(TargetLocation, TargetRow, TargetCol)) {
		if (!IsBlockingObject(TargetRow, TargetCol)) {
			MovedCharacter->AddGridMovementInput(GetGridLocation(TargetRow, TargetCol));
			return true;
		}
	}

	return false;
}


bool AGridScene::InsertObjectIntoCrossList(IGridPropertyInterface* Object, int32 Row, int32 Col) {
	if (IsBlockingObject(Row, Col) && Object->IsBlockTheCharacter()) return false;

	FStorageObjectList* NewObjectPtr = new FStorageObjectList(Row, Col, Object);
	if (ObjectRowList[Row] != nullptr) {
		FStorageObjectList* FindThePlaceToInsert = ObjectRowList[Row];
		if (Col >= FindThePlaceToInsert->Col) {
			// find the place
			while (FindThePlaceToInsert->NextObjectInRow != nullptr && FindThePlaceToInsert->NextObjectInRow->Col <= Col) {
				FindThePlaceToInsert = FindThePlaceToInsert->NextObjectInRow;
			}
			NewObjectPtr->NextObjectInRow = FindThePlaceToInsert->NextObjectInRow;

			// check whether is overlap 
			if (FindThePlaceToInsert->Col == Col) {
				while (FindThePlaceToInsert->OverlapObject != nullptr) {
					FindThePlaceToInsert = FindThePlaceToInsert->OverlapObject;
				}
				FindThePlaceToInsert->OverlapObject = NewObjectPtr;
				NewObjectPtr->NextObjectInCol = FindThePlaceToInsert->NextObjectInCol;
			}
			else{
				FindThePlaceToInsert->NextObjectInRow = NewObjectPtr;
			}
		}
		else {
			NewObjectPtr->NextObjectInRow = FindThePlaceToInsert;
			ObjectRowList[Row] = NewObjectPtr;
		}
	}
	else{
		ObjectRowList[Row] = NewObjectPtr;
	}

	if (ObjectColList[Col] != nullptr) {
		FStorageObjectList* FindThePlaceToInsert = ObjectColList[Col];
		if (Row >= FindThePlaceToInsert->Row) {
			// find the place
			while (FindThePlaceToInsert->NextObjectInCol != nullptr && FindThePlaceToInsert->NextObjectInCol->Row <= Row) {
				FindThePlaceToInsert = FindThePlaceToInsert->NextObjectInCol;
			}

			// check whether is overlap 
			if (FindThePlaceToInsert->Row != Row) {
				NewObjectPtr->NextObjectInCol = FindThePlaceToInsert->NextObjectInCol;
				FindThePlaceToInsert->NextObjectInCol = NewObjectPtr;
			}
		}
		else {
			NewObjectPtr->NextObjectInCol = FindThePlaceToInsert;
			ObjectColList[Col] = NewObjectPtr;
		}
	}
	else {
		ObjectColList[Col] = NewObjectPtr;
	}


	ObjectColNum = (Col > ObjectColNum) ? Col : ObjectColNum;
	ObjectRowNum = (Row > ObjectRowNum) ? Row : ObjectRowNum;
	ObjectTotalNum++;

	return true;
}

void AGridScene::EnableCharacterBattleInput(bool NewState){
	for (auto &i : CharacterArray) {
		i.OrderProcessComponent->SetEnableBattleInput(NewState);
	}
}


bool AGridScene::OrderProcess(){
	bool bHaveOrder = false;
	for (FStorageCharacter& CharacterStruct : CharacterArray) {
		if (!CharacterStruct.OrderProcessComponent->GetOrderExecuting()) {
			if(CharacterStruct.OrderProcessComponent->GetCurrentOrderNum() > 0) {
				bHaveOrder = true;
				const FOrderInput CurrentOrder = CharacterStruct.OrderProcessComponent->GetFirstOrder();
				const FVector CurrentLoc = CharacterStruct.TheCharacter->GetActorLocation();
				const FVector TargetLoc = CurrentOrder.TargetLocation;

				switch (CurrentOrder.OrderType)
				{
				case EOrderType::EMoveOrder:
					if (CanCharacterMoveTo(CurrentLoc, TargetLoc)) {
						CharacterStruct.OrderProcessComponent->ExecuteFirstOrder();
					}
					break;
				default:
					CharacterStruct.OrderProcessComponent->ExecuteFirstOrder();
					break;
				}
			}
		}
		else {
			bHaveOrder = true;
		}
	}
	return bHaveOrder;
}

bool AGridScene::CanCharacterMoveTo(const FVector &CurrentLocation, const FVector &TargetLocation){
	int32 TargetRow = 0;
	int32 TargetCol = 0;
	int32 CurrentRow = 0;
	int32 CurrentCol = 0;
	if (GetGridPosition(TargetLocation, TargetRow, TargetCol)) {
		if (GetGridPosition(CurrentLocation, CurrentRow, CurrentCol)) {
			int32 DeltaRow = TargetRow - CurrentRow;
			int32 DeltaCol = TargetCol - CurrentCol;
			if (IsInRange(TargetRow, TargetRow) && !IsBlockingObject(TargetRow, TargetCol)) {
				if (abs(DeltaRow) <= 1 && abs(DeltaCol) <= 1) return true;
			}
		}
	}
	return false;
}

FOrderInput AGridScene::GetLatestOrder(ATurnBaseCharacter * TheCharacter)
{
	int32 index = SearchCharacter(TheCharacter);
	if (index >= 0) {
		return CharacterArray[index].OrderProcessComponent->GetLatestOrder();
	}

	return FOrderInput();
}

void AGridScene::ClearLatestOrder(ATurnBaseCharacter * TheCharacter){
	int32 index = SearchCharacter(TheCharacter);
	if (index >= 0) {
		CharacterArray[index].OrderProcessComponent->ClearLatestOrder();
	}
}

void AGridScene::DeleteStorageObject(int32 Row, int32 Col) {
	FStorageObjectList* FindPrePtr = ObjectColList[Col];
	FStorageObjectList* TargetPtr;

	if (FindPrePtr == nullptr) return;

	if (FindPrePtr->Row == Row) {
		ObjectColList[Col] = FindPrePtr->NextObjectInCol;
	}
	else {
		while (FindPrePtr->NextObjectInCol != nullptr && FindPrePtr->NextObjectInCol->Row != Row) {
			FindPrePtr = FindPrePtr->NextObjectInCol;
		}

		FindPrePtr->NextObjectInCol = FindPrePtr->NextObjectInCol->NextObjectInCol;
	}

	FindPrePtr = ObjectRowList[Row];
	if (FindPrePtr == nullptr) return;

	if (FindPrePtr->Col == Col) {
		TargetPtr = FindPrePtr;
		ObjectRowList[Row] = FindPrePtr->NextObjectInRow;
	}
	else {
		while (FindPrePtr->NextObjectInRow != nullptr && FindPrePtr->NextObjectInRow->Col != Col) {

			FindPrePtr = FindPrePtr->NextObjectInRow;
		}

		TargetPtr = FindPrePtr->NextObjectInRow;
		 if (TargetPtr == nullptr) return;
		FindPrePtr->NextObjectInRow = TargetPtr->NextObjectInRow;
	}
	DeleteAllTheOverLapObject(TargetPtr);
}

void AGridScene::DeleteAllTheOverLapObject(FStorageObjectList * DeletePtr) {
	if (DeletePtr == nullptr) return;
	if (DeletePtr->OverlapObject != nullptr) {
		DeleteAllTheOverLapObject(DeletePtr->OverlapObject);
	}
	delete DeletePtr;
}

void AGridScene::DeleteTheColList(FStorageObjectList * DeletePtr) {
	if (DeletePtr == nullptr) return;
	if (DeletePtr->NextObjectInCol != nullptr) {
		DeleteTheColList(DeletePtr->NextObjectInCol);
	}
	delete DeletePtr;
}

bool AGridScene::PathSearch(TArray<FVector2D> &Path, int32 CurrentRow, int32 CurrentCol, int32 TargetRow, int32 TargetCol) {
	if (CurrentRow == TargetRow && CurrentCol == TargetCol) return false;
	Path.Init(FVector2D::ZeroVector, abs(TargetRow - CurrentRow) + abs(TargetCol - CurrentCol));
	int32 index = 0;

	return Searching(Path, CurrentRow, CurrentCol, TargetRow, TargetCol, index);
}

bool AGridScene::Searching(TArray<FVector2D> &Path, int32 CurrentRow, int32 CurrentCol, int32 TargetRow, int32 TargetCol, int32 index) {
	// wait for inplementation
	return false;
}

int32 AGridScene::SearchCharacter(ATurnBaseCharacter * SearchTarget)
{
	int32 index = -1;
	for (int32 i = 0; i < CharacterArray.Num(); i++) {
		if (CharacterArray[i].TheCharacter == SearchTarget) {
			index = i;
			break;
		}
	}
	return index;
}

bool AGridScene::UpdateGrid(ATurnBaseCharacter* MovedCharacter, const FVector& PreLocation) {
	IGridPropertyInterface* TestInterface = Cast<IGridPropertyInterface>(MovedCharacter);
	int32 PreRow = 0;
	int32 PreCol = 0;
	GetGridPosition(PreLocation, PreRow, PreCol);
	if (TestInterface == nullptr || !IsInRange(PreRow, PreCol)) return false;

	int32 CurrentRow = 0;
	int32 CurrentCol = 0;
	GetGridPosition(MovedCharacter->GetActorLocation(), CurrentRow, CurrentCol);

	int index = -1;

	if (CharacterArray.Find(FStorageCharacter(PreRow, PreCol, MovedCharacter, false), index)) {
		if (!IsInRange(CurrentRow, CurrentCol)) {
			CharacterArray.RemoveAt(index);
			FStorageObjectList* ObjectList = GetGridObject(CurrentRow, CurrentCol);

			if (ObjectList->OverlapObject == nullptr) {
				DeleteStorageObject(CurrentRow, CurrentCol);
			}

			while (ObjectList->OverlapObject != nullptr) {
				if (ObjectList->OverlapObject->StoragedObject == TestInterface) {
					FStorageObjectList* TargetPtr = ObjectList->OverlapObject;
					ObjectList->OverlapObject = TargetPtr->OverlapObject;

					delete TargetPtr;
					break;
				}
			}
		}
		else {
			CharacterArray[index].Row = CurrentRow;
			CharacterArray[index].Col = CurrentCol;
			MoveObjectTo(MovedCharacter, PreRow, PreCol, CurrentRow, CurrentCol, true, false);
		}
	}
	
	return false;
}