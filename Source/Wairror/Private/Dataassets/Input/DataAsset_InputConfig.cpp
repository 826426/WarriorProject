// Fill out your copyright notice in the Description page of Project Settings.


#include "Dataassets/Input/DataAsset_InputConfig.h"

UInputAction* UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InputTag) const
{
	for (const FWarriorInputActionConfig& InputActionConfig : NativeInputActions)
	{
		if (InputActionConfig.InputTag == InputTag)
		{
			return InputActionConfig.InputAction;
		}
	}

	return nullptr;
}
