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

	UPROPERTY()
		class UProceduralMeshComponent* ProceduralMesh;

	// the color of line
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor LineColor;

		// the opacity of line, this value should be in range of 0 to 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float LineOpacity;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	/**
	* Create Grid and line Material instance with color and opcatity.
	* @param  SourceMaterial   the Material use to create
	* @param  Color            the color use to set material
	* @param  Opacity          the Opcity use to set material
	*/
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		UMaterialInstanceDynamic* CreateMaterialInstance(const UMaterialInterface *SourceMaterial, const FLinearColor &Color, float Opacity);

	/**
	* Create Grid and line Material instance with default setting.
	* @param  SourceMaterial   the Material use to create
	*/
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		UMaterialInstanceDynamic* CreateDefaultMaterialInstance(const UMaterialInterface *SourceMaterial);
};
