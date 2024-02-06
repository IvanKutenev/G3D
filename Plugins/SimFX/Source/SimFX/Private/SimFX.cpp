// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimFX.h"

#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "RHI.h"
#include "GlobalShader.h"
#include "RHICommandList.h"
#include "RenderGraphBuilder.h"
#include "RenderTargetPool.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FSimFXModule"

void FSimFXModule::StartupModule()
{
	FString PluginShadersDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("SimFX"))->GetBaseDir(), TEXT("Source/SimFX/Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Shaders"), PluginShadersDir);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimFXModule, SimFX)