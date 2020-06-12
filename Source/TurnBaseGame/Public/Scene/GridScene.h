// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrderInput.h"
#include "GridScene.generated.h"

class IGridPropertyInterface;
class ATurnBaseCharacter;
class UOrderProcessComponent;
/*
	GridScene is used to create base scene with grid. You can choose the rows, columns, tile size and other property. the AGridScene limit character movement.
*/

USTRUCT(BlueprintType)
struct FStorageObjectList
{
	GENERATED_USTRUCT_BODY()
public:
	int32 Row;
	int32 Col;
	IGridPropertyInterface* StoragedObject;
	FStorageObjectList* NextObjectInRow;
	FStorageObjectList* NextObjectInCol;
	FStorageObjectList* OverlapObject;
	FStorageObjectList();
	FStorageObjectList(int32 row, int32 col, IGridPropertyInterface* storagedObject, FStorageObjectList* nextObjectInRow = nullptr, 
		FStorageObjectList* nextObjectInCol = nullptr, FStorageObjectList* overlapObject = nullptr);
};

USTRUCT(BlueprintType)
struct FStorageCharacter
{
	GENERATED_USTRUCT_BODY()
public:
	int32 Row;
	int32 Col;
	ACharacter* TheCharacter;
	UOrderProcessComponent* OrderProcessComponent;
	bool IsPlayerCharacter;
	FStorageCharacter();
	FStorageCharacter(int32 row, int32 col, ACharacter* theCharacter, bool isPlayerCharacter);

	friend bool operator==(const FStorageCharacter &A, const FStorageCharacter &B);
};

USTRUCT(BlueprintType)
struct FPathSearchNode
{
	GENERATED_USTRUCT_BODY()
public:
	bool IsActive;
	int32 Row;
	int32 Col;
	int32 GCost;
	int32 HCost;
	int32 TotalCost;
	FPathSearchNode* ParentNode;
	FPathSearchNode();
	FPathSearchNode(int32 row, int32 col);
	FPathSearchNode(int32 row, int32 col, int32 gCost, int32 hCost, FPathSearchNode* parentNode);
};

UCLASS()
class TURNBASEGAME_API AGridScene : public AActor
{
	GENERATED_BODY()

	friend class ATurnBaseGameModeBase;

public:	
	// Sets default values for this actor's properties
	AGridScene();
	~AGridScene();

	// use proceddural mesh component to draw lines
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		class UProceduralMeshComponent* LineProceduralMesh;

	// use proceddural mesh component to draw a tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		class UProceduralMeshComponent* TileProceduralMesh;

	// the color of line
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		FLinearColor LineColor;

	// the color of tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		FLinearColor TileColor;

	// the opacity of line, this value should be in range of 0 to 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float LineOpacity;

	// the thickness of line
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		float LineThickness;

	// the opacity of Tile, this value should be in range of 0 to 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float TileOpacity;
	
	// the material of line
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		UMaterialInterface *LineMaterial;

	// the material of line
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		UMaterialInterface *TileMaterial;

	// the rows of tile
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Property")
		int32 TileRow;

	// the Columns of tile
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Property")
		int32 TileCol;

	// the size of tile
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Property")
		float TileSize;

	bool bOrderProcess;

	TArray<FStorageCharacter> CharacterArray;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StorageObject", meta = (AllowprivateAccess = "true"))
		int32 ObjectTotalNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StorageObject", meta = (AllowprivateAccess = "true"))
		int32 ObjectRowNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StorageObject", meta = (AllowprivateAccess = "true"))
		int32 ObjectColNum;

	TArray<FStorageObjectList*> ObjectRowList;
	TArray<FStorageObjectList*> ObjectColList;


	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StorageObject", meta = (AllowprivateAccess = "true"))
	//	ATurnBasePlayerCharacter* CurrentPlayerCharacter;

public:	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The function get the width of grid. 
	UFUNCTION(BlueprintPure, Category = "Property")
		FORCEINLINE float GetGridWidth() const { return TileSize * TileCol; }

	// The function get the height of grid. 
	UFUNCTION(BlueprintPure, Category = "Property")
		FORCEINLINE float GetGridHeight() const { return TileSize * TileRow; }

	// Check the rows and cols is valid or not.
	UFUNCTION(BlueprintPure, Category = "Grid")
		bool IsInRange(int32 Row, int32 Col);

	/**
	* For the Given location return the row and col in the grid.
	* @param  Location         the World Location
	* @param  Row			   return the row
	* @param  Col			   return the col
	* return                   Is it the Location in the Grid
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid")
		bool GetGridPosition(FVector Location, int32 &Row, int32 &Col);

	/**
	* For the Given Index return the location.
	* @param  Row			   input row
	* @param  Col			   input col
	* @param  IsCenter		   if value is true return the Center Location, Value is false return Left-down Location Value 
	* return                   The location that the index point to. No matter what index was given.
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid")
		FVector GetGridLocation(int32 Row, int32 Col, bool IsCenter = true);

	/**
	* Get the Center location of grid
	* @param   Location       Current Location
	* @return  The center location
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid")
		FVector GetGridCenter(const FVector& Location);

	/**
	* Add a new object into Cross Lists
	* @param  NewGridObject  the object wait for add
	* @return            is it add successfully
	*/
	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		bool AddObjectIntoGrid(AActor* NewGridObject);

	/**
	* check whether has a block object on a tile
	* @param  Row		Row of Object
	* @param  Col		Col of Object
	*/
	UFUNCTION(BlueprintPure, Category = "StorageObject")
		bool IsBlockingObject(int32 Row, int32 Col);

	/**
	* Move the given object to the appointed position, the object show have the interface of IGridPropertyInterface.
	* @param  MovedObject      the object need to move
	* @param  Row			   which row does the object move to
	* @param  Col			   which col does the object move to
	* @param  IsCenter		   Does it move to the center
	* @param  IsMove		   Should it move
	* @return				   is it moved successfully
	*/
	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		bool MoveObjectTo(AActor* MovedObject, int32 Row, int32 Col, bool IsCenter = true, bool IsMove = true);
	
	// just move object list in grid
	bool MoveObjectTo(AActor* MovedObject,int32 PreRow, int32 PreCol, int32 Row, int32 Col, bool IsCenter = true, bool IsMove = false);

	/**
	* move the given character to the appointed position, the object show have the interface of IGridPropertyInterface. The character can only move one tile.
	* @param	MoveCharacter	The Character need to move.
	* @param	DeltaRow		the delta tile in row
	* @param	DeltaCol		the delta tile in col
	* @return					is it moved successfully
	*/
	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		bool MoveCharacterByDeltaIndex(ATurnBaseCharacter* MovedCharacter, int32 DeltaRow, int32 DeltaCol);

	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		bool MoveCharacterToLocation(ATurnBaseCharacter* MovedCharacter, const FVector &TargetLocation);

	/** Show the Section on the screen
	* @param  ShowLocation		The location want to show
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid")
		void ShowSelectSection(const FVector &ShowLocation);

	/**
	* Search A path to target Row and Col
	* @Param  Path            Storage the  Path
	* @return				  Does it have a road to the target
	*/
	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		bool PathSearch(TArray<FVector2D> &Path, int32 CurrentRow, int32 CurrentCol, int32 TargetRow, int32 TargetCol);

	/**
	* use to update character position in grid
	* @Param  MovedCharacter        the character need to update
	* @Param  PreLocation			the location before update
	* @return						Does it update successfully
	*/
	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		bool UpdateGrid(ATurnBaseCharacter* MovedCharacter, const FVector& PreLocation);

	// enable the ability of character input order in CharacterArray
	UFUNCTION(BlueprintCallable, Category = "StorageObject")
		void EnableCharacterBattleInput(bool NewState);

	UFUNCTION(BlueprintCallable, Category = "Order")
		bool OrderProcess();

	UFUNCTION(BlueprintCallable, Category = "Order")
		bool CanCharacterMoveTo(const FVector &CurrentLocation, const FVector &TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Order")
		FOrderInput GetLatestOrder(ATurnBaseCharacter* TheCharacter);

	// clear the latest order
	UFUNCTION(BlueprintCallable, Category = "Order")
		void ClearLatestOrder(ATurnBaseCharacter* TheCharacter);


	void DeleteStorageObject(int32 Row, int32 Col);

	FStorageObjectList* GetGridObject(int32 Row, int32 Col);

protected:
	
	void ClearAllShadowCharacter();

	/**
	* Insert a object into the cross list
	*/
	bool InsertObjectIntoCrossList(IGridPropertyInterface* Object, int32 Row, int32 Col);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DeleteAllTheOverLapObject(FStorageObjectList* DeletePtr);

private:
	/**
	* Create Grid and line Material instance with color and opcatity.
	* @param  SourceMaterial   the Material use to create
	* @param  Color            the color use to set material
	* @param  Opacity          the Opcity use to set material
	*/
	UFUNCTION(BlueprintCallable, Category = "Property", meta = (AllowPrivateAccess = "true"))
		UMaterialInstanceDynamic* CreateMaterialInstance(UMaterialInterface *SourceMaterial,const FLinearColor &Color, float Opacity);

	/**
	* Create Grid and line Material instance with default setting.
	* @param  SourceMaterial   the Material use to create
	*/
	UFUNCTION(BlueprintCallable, Category = "Property", meta = (AllowPrivateAccess = "true"))
		UMaterialInstanceDynamic* CreateDefaultMaterialInstance(UMaterialInterface *SourceMaterial);

	/**
	*  Prepare for creating line. create Vertices and Triangles Array for UProceduralMeshComponent::CreateMeshSection 
	*  @param  Vertices    should be an empty array for storage vertices
	*  @param  Triangles   should be an empty array for storage triangles
	*/
	UFUNCTION(BlueprintCallable, Category = "Property", meta = (AllowPrivateAccess = "true"))
		void CreateLine(TArray<FVector> &Vertices, TArray<int32>& Triangles);

	/**
	*  Prepare for creating tile. create Vertices and Triangles Array for UProceduralMeshComponent::CreateMeshSection
	*  @param  Vertices    should be an empty array for storage vertices
	*  @param  Triangles   should be an empty array for storage triangles
	*/
	UFUNCTION(BlueprintCallable, Category = "Property", meta = (AllowPrivateAccess = "true"))
		void CreateTile(TArray<FVector> &Vertices, TArray<int32>& Triangles);

	/**
	* The function use to delete the whole Col list.
	* @param  DeletePtr   The List need to be delete
	*/
	void DeleteTheColList(FStorageObjectList * DeletePtr);

	// use recursion to search
	bool Searching(FPathSearchNode * CurrentNode, int32 TargetRow, int32 TargetCol, FVector2D Direction, FPathSearchNode * ReturnNode);

	void CalcPath(TArray<FPathSearchNode*> OpenList, int32 Index, TArray<FVector2D> &Path);

	// search target character in CharacterArray.If found return index, not found return -1.
	int32 SearchCharacter(ATurnBaseCharacter* SearchTarget);
};
