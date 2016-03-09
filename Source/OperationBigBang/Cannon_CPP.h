// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Cannon_CPP.generated.h"

UCLASS()
class OPERATIONBIGBANG_API ACannon_CPP : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACannon_CPP();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
	// Called via Blueprint
	UFUNCTION (BlueprintCallable, Category=Calculate)
	FVector LocationCalc(FVector initialLocation);
	
};
