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

	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		FVector currentLocation = Pawn->GetActorLocation();
		fVelocity += fAcceleration * DeltaTime;
		currentLocation.Y += fVelocity;
		Pawn->SetActorLocation(currentLocation);
		if (fAcceleration <= 0.05f && fAcceleration > -0.05f)
		{
			fAcceleration = 0.0f;
		}
		else
		{
			fAcceleration *= 0.95f;
		}
		if (fVelocity <= 0.05f && fVelocity > -0.05f)
		{
			fVelocity = 0.0f;
		}
		else
		{
			fVelocity *= 0.95;
		}
	}
}

void AOperationBigBangPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AOperationBigBangPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AOperationBigBangPlayerController::OnSetDestinationReleased);

	InputComponent->BindAxis("LeftThumbXAxis", this, &AOperationBigBangPlayerController::OnLeftStick);
	InputComponent->BindAxis("LeftThumbYAxis", this, &AOperationBigBangPlayerController::OnLeftStick);

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

void AOperationBigBangPlayerController::OnLeftStick(float val)
{
	this->OnLeftStick();
}

void AOperationBigBangPlayerController::OnLeftStick()
{
	FVector2D currentAngleVector(InputComponent->GetAxisValue("LeftThumbXAxis"),
		InputComponent->GetAxisValue("LeftThumbYAxis"));
	currentAngleVector.Normalize();
	// Check if Left Thumbstick is released
	if (currentAngleVector.Size() <= 0.05f && currentAngleVector.Size() >= -0.05f)
	{
		//Stick in Resting position
		return;
	}

	float direction = 1.0f;
	// Crossproduct == 0 is exactly 180 degree rotated | < 0 is right of the old vector | > 0 is left of the old vector.
	float crossProd = FVector2D::CrossProduct(oldLeftThumbAngleVector, currentAngleVector);
	if (crossProd < 0.0f)
	{
		direction = 1.0f;
	}
	else if (crossProd > 0.0f)
	{
		direction = -1.0f;
	}
	else
	{
		//No direction no update!
		return;
	}
	// Determine delta angle
	float deltaAngle = FMath::Acos(FVector2D::DotProduct(oldLeftThumbAngleVector, currentAngleVector));
	//UE_LOG(TestLog, Error, TEXT("dA:%f"), deltaAngle);
	float tempAccel = direction * deltaAngle * 5.0f;
	//UE_LOG(TestLog, Error, TEXT("L:%d|N:%d|R:%d"), left, none, right);
	//UE_LOG(TestLog, Error, TEXT("fAccel: %f|%f tAcc| deltaA: %f"), fAcceleration, tempAccel, deltaAngle);

	fAcceleration += tempAccel;
	oldLeftThumbAngleVector = currentAngleVector;
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
