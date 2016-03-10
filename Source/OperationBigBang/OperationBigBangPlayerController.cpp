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
		//fVelocity += fAcceleration * DeltaTime;
		velocity.Y = acceleration.Y * DeltaTime;
		velocity.X = acceleration.X * DeltaTime;
		currentLocation.X += velocity.X;
		currentLocation.Y += velocity.Y;
		Pawn->SetActorLocation(currentLocation);
		dampenAcceleration();
		dampenVelocity();
	}
}

void AOperationBigBangPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("WKeyPress", IE_Pressed, this, &AOperationBigBangPlayerController::OnWPress);
	InputComponent->BindAction("SKeyPress", IE_Released, this, &AOperationBigBangPlayerController::OnSPress);
	InputComponent->BindAction("AKeyPress", IE_Pressed, this, &AOperationBigBangPlayerController::OnAPress);
	InputComponent->BindAction("DKeyPress", IE_Released, this, &AOperationBigBangPlayerController::OnDPress);

	// Bind Stick Rotations
	InputComponent->BindAxis("LeftThumbXAxis", this, &AOperationBigBangPlayerController::OnLeftStick);
	InputComponent->BindAxis("LeftThumbYAxis", this, &AOperationBigBangPlayerController::OnLeftStick);

	InputComponent->BindAxis("RightThumbXAxis", this, &AOperationBigBangPlayerController::OnRightStick);
	InputComponent->BindAxis("RightThumbYAxis", this, &AOperationBigBangPlayerController::OnRightStick);

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
		return;
	}
	// Determine delta angle
	float deltaAngle = FMath::Acos(FVector2D::DotProduct(oldLeftThumbAngleVector, currentAngleVector));
	//UE_LOG(TestLog, Error, TEXT("dA:%f"), deltaAngle);
	float tempAccel = direction * deltaAngle * 25.0f;
	//UE_LOG(TestLog, Error, TEXT("L:%d|N:%d|R:%d"), left, none, right);
	//UE_LOG(TestLog, Error, TEXT("fAccel: %f|%f tAcc| deltaA: %f"), fAcceleration, tempAccel, deltaAngle);

	acceleration.Y += tempAccel;
	oldLeftThumbAngleVector = currentAngleVector;
}

void AOperationBigBangPlayerController::OnRightStick(float val)
{
	this->OnRightStick();
}

void AOperationBigBangPlayerController::OnRightStick()
{
	FVector2D currentAngleVector(	InputComponent->GetAxisValue("RightThumbXAxis"),
									InputComponent->GetAxisValue("RightThumbYAxis"));
	currentAngleVector.Normalize();
	// Check if Left Thumbstick is released
	if (currentAngleVector.Size() <= 0.05f && currentAngleVector.Size() >= -0.05f)
	{
		//Stick in Resting position
		return;
	}

	float direction = 1.0f;
	// Crossproduct == 0 is exactly 180 degree rotated | < 0 is right of the old vector | > 0 is left of the old vector.
	float crossProd = FVector2D::CrossProduct(oldRightThumbAngleVector, currentAngleVector);
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
		return;
	}
	// Determine delta angle
	float deltaAngle = FMath::Acos(FVector2D::DotProduct(oldRightThumbAngleVector, currentAngleVector));
	// calc acceleration
	float tempAccel = direction * deltaAngle * 25.0f;

	acceleration.X += tempAccel;
	oldRightThumbAngleVector = currentAngleVector;
}

void AOperationBigBangPlayerController::dampenVelocity()
{
	if (velocity.X <= 0.05f && velocity.X > -0.05f)
	{
		velocity.X = 0.0f;
	}
	else
	{
		velocity.X *= 0.95;
	}

	if (velocity.Y <= 0.05f && velocity.Y > -0.05f)
	{
		velocity.Y = 0.0f;
	}
	else
	{
		velocity.Y *= 0.95;
	}
}

void AOperationBigBangPlayerController::dampenAcceleration()
{
	if (acceleration.X <= 0.05f && acceleration.X > -0.5f)
	{
		acceleration.X = 0.0f;
	}
	else
	{
		acceleration.X *= 0.95;
	}
	if (acceleration.Y <= 0.05f && acceleration.Y > -0.5f)
	{
		acceleration.Y = 0.0f;
	}
	else
	{
		acceleration.Y *= 0.95;
	}
}

void AOperationBigBangPlayerController::OnWPress()
{
	// set flag to keep updating destination until released
	acceleration.X += 50.0f;
}

void AOperationBigBangPlayerController::OnSPress()
{
	// clear flag to indicate we should stop updating the destination
	acceleration.X -= 50.0f;
}

void AOperationBigBangPlayerController::OnAPress()
{
	// set flag to keep updating destination until released
	acceleration.Y -= 50.0f;
}

void AOperationBigBangPlayerController::OnDPress()
{
	// clear flag to indicate we should stop updating the destination
	acceleration.Y += 50.0f;
}