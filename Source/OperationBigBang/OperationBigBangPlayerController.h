// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "OperationBigBangPlayerController.generated.h"

UCLASS()
class AOperationBigBangPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOperationBigBangPlayerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;
	/** Stores the old angle of the left Thumbstick. */
	FVector2D oldLeftThumbAngleVector = FVector2D(0.0f, 1.0f);
	/** Stores the old angle of the right Thumbstick. */
	FVector2D oldRightThumbAngleVector = FVector2D(0.0f, 1.0f);
	/** Current velocity of the crosshair. */
	FVector2D velocity = FVector2D(0.0f, 0.0f);
	/** Current acceleration of the crosshair. */
	FVector2D acceleration = FVector2D(0.0f, 0.0f);


	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	void OnLeftStick(float val);
	void OnLeftStick();

	void OnRightStick(float val);
	void OnRightStick();

	void dampenVelocity();
	void dampenAcceleration();
	/** Input handlers for SetDestination action. */
	void OnWPress();
	void OnSPress();
	void OnAPress();
	void OnDPress();
};


