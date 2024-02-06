// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaterSystem.h"

void FWaterSystem::Dispatch(FPostOpaqueRenderParameters& Parameters)
{
	FScopeLock ParamsLock(&DispatchParamsGuard);
	if (!IsValid(DispatchParams.WaterHeightmapRT))
	{
		return;
	}
	return;
}