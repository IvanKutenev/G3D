#include "SimulationSubsystem.h"

#include "SimFX.h"
#include "UObject/ConstructorHelpers.h"

namespace FSimulationSubsystemLocal
{
	TCHAR* TerrainHeightMapTexFilename = TEXT("/Game/Maps/T_TerrainH.T_TerrainH");
	TCHAR* WaterHeightMapRTFilename = TEXT("/Game/Maps/RT_WaterH.RT_WaterH");
	TCHAR* WaterFlowMapRTFilename = TEXT("/Game/Maps/RT_WaterV.RT_WaterV");
	TCHAR* WaterNormalMapRTFilename = TEXT("/Game/Maps/RT_WaterN.RT_WaterN");
	TCHAR* WaterFoamMapRTFilename = TEXT("/Game/Maps/RT_FoamMask.RT_FoamMask");
};

USimulationSubsystem::USimulationSubsystem()
{
	TerrainHeightMapTex = ConstructorHelpers::FObjectFinderOptional<UTexture2D>(
		FSimulationSubsystemLocal::TerrainHeightMapTexFilename).Get();

	WaterHeightMapRT = ConstructorHelpers::FObjectFinderOptional<UTextureRenderTarget2D>(
		FSimulationSubsystemLocal::WaterHeightMapRTFilename).Get();

	WaterFlowMapRT = ConstructorHelpers::FObjectFinderOptional<UTextureRenderTarget2D>(
		FSimulationSubsystemLocal::WaterFlowMapRTFilename).Get();

	WaterNormalMapRT = ConstructorHelpers::FObjectFinderOptional<UTextureRenderTarget2D>(
		FSimulationSubsystemLocal::WaterNormalMapRTFilename).Get();

	WaterFoamMapRT = ConstructorHelpers::FObjectFinderOptional<UTextureRenderTarget2D>(
		FSimulationSubsystemLocal::WaterFoamMapRTFilename).Get();

	if (auto SimFxModule = FSimFXModule::Get())
	{
		SimFxModule->WaterSystem.SetTerrainHeightMapTex(TerrainHeightMapTex);
		SimFxModule->WaterSystem.SetWaterHeightMapRT(WaterHeightMapRT);
		SimFxModule->WaterSystem.SetWaterFlowMapRT(WaterHeightMapRT);
		SimFxModule->WaterSystem.SetWaterNormalMapRT(WaterHeightMapRT);
		SimFxModule->WaterSystem.SetWaterFoamMapRT(WaterHeightMapRT);
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