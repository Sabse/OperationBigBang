// Fill out your copyright notice in the Description page of Project Settings.

#include "OperationBigBang.h"
#include "Cannon_CPP.h"


// Sets default values
ACannon_CPP::ACannon_CPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACannon_CPP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACannon_CPP::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Calculate Offset for Projectile to start
FVector ACannon_CPP::LocationCalc(FVector initialLocation)
{
	
	FVector newLocation = initialLocation;
	return newLocation;
}

