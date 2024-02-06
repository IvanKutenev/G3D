// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "WaterSystem.h"

#include "RenderGraphResources.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"

class FSimFXModule : public IModuleInterface
{
public:
	static FSimFXModule* Get()
	{
		return (FSimFXModule*)FModuleManager::Get().GetModule("SimFX");
	}
	virtual void StartupModule() override;

public:
	FWaterSystem WaterSystem;
};
