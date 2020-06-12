// Fill out your copyright notice in the Description page of Project Settings.


#include "GridScene.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "GridPropertyInterface.h"
#include "TurnBaseCharacter.h"
#include "Algo/Reverse.h"
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

FPathSearchNode::FPathSearchNode()
	: IsActive(true)
	, Row(0)
	, Col(0)
	, GCost(0)
	, HCost(10)
	, TotalCost(10)
	, ParentNode(nullptr)
{

}

FPathSearchNode::FPathSearchNode(int32 row, int32 col)
	: IsActive(true)
	, Row(row)
	, Col(col)
	, GCost(0)
	, HCost(10)
	, TotalCost(10)
	, ParentNode(nullptr)
{
}


FPathSearchNode::FPathSearchNode(int32 row, int32 col, int32 gCost, int32 hCost, FPathSearchNode* parentNode)
	: IsActive(true)
	, Row(row)
	, Col(col)
	, GCost(gCost)
	, HCost(hCost)
	, TotalCost(gCost + hCost)
	, ParentNode(parentNode)
{
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

bool AGridScene::GetGridPosition(FVector Location, int32 &Row, int32 &Col) {
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
	if (!IsInRange(Row, Col)) return nullptr;
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
	TArray<FPathSearchNode*> OpenList;
	if (IsInRange(CurrentRow, CurrentCol)) {
		// HCost function
		auto HCostFunc = [TargetRow, TargetCol](int32 Row, int32 Col) {
			return int32((abs(TargetCol - Col) + abs(TargetRow - Row)) * 10);
		};
		// Search the same node in the list

		auto SGN = [](int32 a) {
			if (a > 0) return 1;
			else if (a < 0) return -1;
			else return 0;
		};

		auto ActiveCalc = [&OpenList]() {
			int num = 0;
			for (auto &i : OpenList) {
				if (i->IsActive == true)
					num++;
			}
			return num;
		};

		OpenList.Add(new FPathSearchNode(CurrentRow, CurrentCol, 0, HCostFunc(CurrentRow, CurrentCol), nullptr));
		while (ActiveCalc() > 0)
		{
			int index = 0;
			int32 MinCost = OpenList[0]->TotalCost + 10000;
			for (int i = 0; i < OpenList.Num(); i++) {
				if (MinCost > OpenList[i]->TotalCost && OpenList[i]->IsActive) {
					MinCost = OpenList[i]->TotalCost;
					index = i;
				}
				if (OpenList[i]->HCost == 0)
				{
					CalcPath(OpenList, i, Path);
					return true;
				}
			}


			FPathSearchNode* ActiveNode = OpenList[index];
			FPathSearchNode* NewNode = new FPathSearchNode();
			if (ActiveNode->ParentNode == nullptr) {
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (x == 0 && y == 0) continue;
						//如果该节点是父节点就往8个方向搜
						if (Searching(ActiveNode, TargetRow, TargetCol, FVector2D(x, y), NewNode)) {
							OpenList.Add(NewNode);
							NewNode = new FPathSearchNode();
						}
					}
				}
			}
			else{
				//如果是子节点就往父节点相反的三个方向搜索
				int32 x = SGN(ActiveNode->Col - ActiveNode->ParentNode->Col);
				int32 y = SGN(ActiveNode->Row - ActiveNode->ParentNode->Row);
				if (x != 0 || y != 0)
				{
					//完全相反反向搜索
					if (Searching(ActiveNode, TargetRow, TargetCol, FVector2D(x, y), NewNode)) {
						OpenList.Add(NewNode);
						NewNode = new FPathSearchNode();
					}
					//计算逆时针和顺时针旋转45°的方向，进行搜索
					if (x == 0)
					{
						if (Searching(ActiveNode, TargetRow, TargetCol, FVector2D(1, y), NewNode)) {
							OpenList.Add(NewNode);
							NewNode = new FPathSearchNode();
						}
						if (Searching(ActiveNode, TargetRow, TargetCol, FVector2D(-1, y), NewNode)) {
							OpenList.Add(NewNode);
							NewNode = new FPathSearchNode();
						}
					}
					else if (y == 0) {
						if (Searching(ActiveNode, TargetRow, TargetCol, FVector2D(x, 1), NewNode)) {
							OpenList.Add(NewNode);
							NewNode = new FPathSearchNode();
						}
						if (Searching(ActiveNode, TargetRow, TargetCol, FVector2D(x, -1), NewNode)) {
							OpenList.Add(NewNode);
							NewNode = new FPathSearchNode();
						}
					}
					else
					{
						if (Searching(ActiveNode, TargetRow, TargetCol, FVector2D(x, 0), NewNode)) {
							OpenList.Add(NewNode);
							NewNode = new FPathSearchNode();
						}
						if (Searching(ActiveNode, TargetRow, TargetCol, FVector2D(0, y), NewNode)) {
							OpenList.Add(NewNode);
							NewNode = new FPathSearchNode();
						}
					}
					//如果斜着走，搜索多两个垂直方向
					if (abs(x + y) != 1) {
						if (Searching(ActiveNode, TargetRow, TargetCol, FVector2D(x, -y), NewNode)) {
							OpenList.Add(NewNode);
							NewNode = new FPathSearchNode();
						}
						if (Searching(ActiveNode, TargetRow, TargetCol, FVector2D(-x, y), NewNode)) {
							OpenList.Add(NewNode);
							NewNode = new FPathSearchNode();
						}
					}
				}
			}
			
			OpenList[index]->IsActive = false;
			delete NewNode;
		}
	}

	return false;
}

bool AGridScene::Searching(FPathSearchNode * CurrentNode, int32 TargetRow, int32 TargetCol, FVector2D Direction, FPathSearchNode * ReturnNode) {
	if (Direction.X == 0 && Direction.Y == 0) {
		return false;
	}

	// wait for implementation
	int32 NewCol = CurrentNode->Col + Direction.X;
	int32 NewRow = CurrentNode->Row + Direction.Y;

	// HCost function
	auto HCostFunc = [TargetRow, TargetCol](int32 Row, int32 Col) {
		return int32((abs(TargetCol - Col) + abs(TargetRow - Row)) * 10);
	};

	auto GCostFunc = [](const FPathSearchNode * CurrentNode, const FPathSearchNode * ReturnNode) {
		if (ReturnNode->Row == CurrentNode->Row || ReturnNode->Col == CurrentNode->Col)
		{
			return CurrentNode->GCost + 10 * (abs(ReturnNode->Row - CurrentNode->Row) + abs(ReturnNode->Row - CurrentNode->Row));
		} 
		else
		{
			return CurrentNode->GCost + 14 * abs(ReturnNode->Row - CurrentNode->Row);
		}
		
	};

	auto ReturnNodeInit = [&ReturnNode, &CurrentNode, &HCostFunc, &GCostFunc](int32 Row, int32 Col) {
		ReturnNode->Row = Row;
		ReturnNode->Col = Col;
		ReturnNode->GCost = GCostFunc(CurrentNode, ReturnNode);
		ReturnNode->HCost = HCostFunc(Row, Col);
		ReturnNode->TotalCost = ReturnNode->GCost + ReturnNode->HCost;
		ReturnNode->ParentNode = CurrentNode;
	};

	while (IsInRange(NewRow, NewCol) && !IsBlockingObject(NewRow, NewCol))
	{
		if (NewRow == TargetRow && NewCol == TargetCol) {
			ReturnNodeInit(TargetRow, TargetCol);
			return true;
		}

		if (abs(Direction.X + Direction.Y) == 1) {

				//分别判断上下两个方向是否有障碍物
				if (IsBlockingObject(NewRow + Direction.X, NewCol + Direction.Y)) {
					//判断斜上下方向是否可以通行来确定是不是强迫邻点
					if (!IsBlockingObject(NewRow + Direction.X + Direction.Y, NewCol + Direction.Y + Direction.X)) {
						ReturnNodeInit(NewRow, NewCol);
						return true;
					}
				}
				if (IsBlockingObject(NewRow - Direction.X, NewCol - Direction.Y)) {
					if (!IsBlockingObject(NewRow - Direction.X + Direction.Y, NewCol - Direction.Y + Direction.X)) {
						ReturnNodeInit(NewRow, NewCol);
						return true;
					}
				}


		}
		else {
			if (IsBlockingObject(NewRow - Direction.Y, NewCol)) {
				if (!IsBlockingObject(NewRow - Direction.Y, NewCol + Direction.X)) {
					ReturnNodeInit(NewRow, NewCol);
					return true;
				}
			}
			if (IsBlockingObject(NewRow, NewCol - Direction.X)) {
				if (!IsBlockingObject(NewRow + Direction.Y, NewCol - Direction.X)) {
					ReturnNodeInit(NewRow, NewCol);
					return true;
				}
			}

			FPathSearchNode CrossSearchNode(NewRow, NewCol);
			if (Searching(&CrossSearchNode, TargetRow, TargetCol, FVector2D(Direction.X, 0), ReturnNode)) {
				ReturnNodeInit(NewRow, NewCol);
				return true;
			}
			if (Searching(&CrossSearchNode, TargetRow, TargetCol, FVector2D(0, Direction.Y), ReturnNode)) {
				ReturnNodeInit(NewRow, NewCol);
				return true;
			}
		}

		NewRow += Direction.Y;
		NewCol += Direction.X;
	}

	return false;
}

void AGridScene::CalcPath(TArray<FPathSearchNode*> OpenList, int32 Index, TArray<FVector2D> &Path)
{
	auto SGN = [](int32 a) {
		if (a > 0) return 1;
		else if (a < 0) return -1;
		else return 0;
	};
	Path.Empty();
	FPathSearchNode Terminus = *OpenList[Index];
	FPathSearchNode Current = Terminus;
	FPathSearchNode Parent = *Terminus.ParentNode;
	int32 Row = Current.Row;
	int32 Col = Current.Col;
	FVector2D Direction = FVector2D(SGN(Current.Col - Parent.Col), SGN(Current.Row - Parent.Row));
	while (Current.ParentNode != nullptr)
	{
		Path.Add(FVector2D(Col, Row));
		UE_LOG(LogTemp, Warning, TEXT("Row: %d, Col: %d"), Row, Col);
		Row -= Direction.Y;
		Col -= Direction.X;

		if (Row == Parent.Row && Col == Parent.Col) {
			Current = Parent;
			if (Parent.ParentNode != nullptr) {
				Parent = *Parent.ParentNode;
			}
			Direction = FVector2D(SGN(Current.Col - Parent.Col), SGN(Current.Row - Parent.Row));
		}
	}
	Algo::Reverse(Path);

	for (auto &i : OpenList) {
		delete i;
	}
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