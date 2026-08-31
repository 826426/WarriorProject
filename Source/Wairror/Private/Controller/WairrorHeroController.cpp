// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/WairrorHeroController.h"

AWairrorHeroController::AWairrorHeroController()
{
	HeroTeamID = FGenericTeamId(0);
}

FGenericTeamId AWairrorHeroController::GetGenericTeamId() const
{
	return HeroTeamID;
}
