// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "OperationBigBang.h"
#include "OperationBigBangPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformMath.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"

AOperationBigBangPlayerController::AOperationBigBangPlayerController()
{

}

void AOperationBigBangPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);



	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		
		FVector currentLocation = Pawn->GetActorLocation();
		//fVelocity += fAcceleration * DeltaTime;
		velocity.Y = acceleration.Y * DeltaTime;
		velocity.X = acceleration.X * DeltaTime;
		currentLocation.X += velocity.X;
		velocity.ClampAxes(-250.0f, 250.0f);
		float halfHeight = (float)(maxHeight) / 2.0f;
		if (currentLocation.X > mapLocation.X + halfHeight)
		{
			currentLocation.X = mapLocation.X + halfHeight;
			velocity.X = 0.0f;
			acceleration.X = 0.0f;
		}
		else if (currentLocation.X < mapLocation.X - halfHeight)
		{
			currentLocation.X = mapLocation.X - halfHeight;
			velocity.X = 0.0f;
			acceleration.X = 0.0f;
		}
		else{}
		currentLocation.Y += velocity.Y;
		float halfWidth = (float)(maxWidth) / 2.0f;
		if (currentLocation.Y > mapLocation.Y + halfWidth)
		{
			currentLocation.Y = mapLocation.Y + halfWidth;
			velocity.Y = 0.0f;
			acceleration.Y = 0.0f;
		}
		else if (currentLocation.Y < mapLocation.Y - halfWidth)
		{
			currentLocation.Y = mapLocation.Y - halfWidth;
			velocity.Y = 0.0f;
			acceleration.Y = 0.0f;
		}
		else {}
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
		acceleration.X *= 0.9f;
	}
	if (acceleration.Y <= 0.05f && acceleration.Y > -0.5f)
	{
		acceleration.Y = 0.0f;
	}
	else
	{
		acceleration.Y *= 0.9f;
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