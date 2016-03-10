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

	FVector2D oldLeftThumbAngleVector = FVector2D(0.0f, 1.0f);
	float fVelocity = 0.0f;
	float fAcceleration = 0.0f;

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

	/** Rotate Mortar around the local z-axis. */
	void OnLeftStick(float val);
	void OnLeftStick();

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
};


