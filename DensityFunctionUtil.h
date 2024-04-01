// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DensityFunctionUtil.generated.h"

#pragma region ENUMS

enum DFType
{
	flat, yEx, circle, simplex, sine, wall, negative, 
	parabaloid, parabaloidwithnoise
};

#pragma endregion ENUMS

UCLASS()
class TERRAUNKNOWN_API UDensityFunctionUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	static float GetDensity(DFType dFType, FVector3f pos, int detail = 0, bool debug = false);

	static float GetDensity(DFType dFType, FVector3d pos, int detail = 0, bool debug = false);

	//Simplex Noise
	static float MOSimplexNoise2D(FVector3d pos, int detail);

	static float SimplexNoise2D(float x, float y);

	static float SimplexNoise3D(float x, float y, float z);
};
