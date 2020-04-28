// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridScene.generated.h"

/*
	GridScene is used to create base scene with grid. You can choose the rows, columns, tile size and other property. the AGridScene limit character movement.
*/




UCLASS()
class TURNBASEGAME_API AGridScene : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridScene();

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		int32 TileRow;

	// the Columns of tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		int32 TileCol;

	// the size of tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
		float TileSize;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void OnConstruction(const FTransform& Transform) override;
	
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
		bool GetGridPosition(const FVector& Location, int32 &Row, int32 &Col);

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


};
