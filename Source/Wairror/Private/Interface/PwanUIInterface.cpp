// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/PwanUIInterface.h"

// Add default functionality here for any IPwanUIInterface functions that are not pure virtual.

UHeroUIComponent* IPwanUIInterface::GetHeroUIComponent() const
{
	return nullptr;
}

UEnemyUIComponent* IPwanUIInterface::GetEnemyUIComponent() const
{
	return nullptr;
}
