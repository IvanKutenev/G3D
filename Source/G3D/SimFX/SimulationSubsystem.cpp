#include "SimulationSubsystem.h"

#include "SimFX.h"
#include "UObject/ConstructorHelpers.h"

namespace FSimulationSubsystemLocal
{
	TCHAR* TerrainHeightmapTexFilename = TEXT("/Game/Maps/T_TerrainH.T_TerrainH");
	TCHAR* WaterHeightmapRTFilename = TEXT("/Game/Maps/RT_WaterH.RT_WaterH");
};

USimulationSubsystem::USimulationSubsystem()
{
	WaterHeightmapRT = ConstructorHelpers::FObjectFinderOptional<UTextureRenderTarget2D>(
		FSimulationSubsystemLocal::WaterHeightmapRTFilename).Get();

	TerrainHeightmapTex = ConstructorHelpers::FObjectFinderOptional<UTexture2D>(
		FSimulationSubsystemLocal::TerrainHeightmapTexFilename).Get();

	if (auto SimFxModule = FSimFXModule::Get())
	{
		SimFxModule->WaterSystem.SetWaterHeightmapRT(WaterHeightmapRT);
		SimFxModule->WaterSystem.SetTerrainHeightmapTex(TerrainHeightmapTex);
	}
}

void USimulationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (auto SimFxModule = FSimFXModule::Get())
	{
		SimFxModule->WaterSystem.Register();
	}
}

void USimulationSubsystem::Deinitialize()
{
	if (auto SimFxModule = FSimFXModule::Get())
	{
		SimFxModule->WaterSystem.Unregister();
	}
}