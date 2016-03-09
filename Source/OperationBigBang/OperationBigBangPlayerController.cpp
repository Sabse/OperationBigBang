// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "OperationBigBang.h"
#include "OperationBigBangPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformMath.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"

AOperationBigBangPlayerController::AOperationBigBangPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AOperationBigBangPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}

	
}

void AOperationBigBangPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AOperationBigBangPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AOperationBigBangPlayerController::OnSetDestinationReleased);

	InputComponent->BindAxis("LeftThumbXAxis", this, &AOperationBigBangPlayerController::RotateSelf);
	InputComponent->BindAxis("LeftThumbYAxis", this, &AOperationBigBangPlayerController::RotateSelf);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AOperationBigBangPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AOperationBigBangPlayerController::MoveToTouchLocation);
}

void AOperationBigBangPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void AOperationBigBangPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AOperationBigBangPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void AOperationBigBangPlayerController::RotateSelf(float val)
{
	this->RotateSelf();
}

void AOperationBigBangPlayerController::RotateSelf()
{
	float x = InputComponent->GetAxisValue("LeftThumbXAxis");
	float y = InputComponent->GetAxisValue("LeftThumbYAxis");
	float angle = FMath::UnwindDegrees(FMath::RadiansToDegrees(FGenericPlatformMath::Atan2(x, y)));

	FRotator rotation = FRotator(0.0f, angle, 0.0f);
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		Pawn->SetActorRotation(rotation);
		//Pawn->SetActorScale3D(FVector(1.0f, 1.0f, angle));
	}
	


}

void AOperationBigBangPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AOperationBigBangPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
