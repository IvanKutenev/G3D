#include "SimulationSubsystem.h"

#include "SimFX.h"
#include "UObject/ConstructorHelpers.h"

namespace FSimulationSubsystemLocal
{
	TCHAR* WaterHeightmapRTFilename = TEXT("/Game/Maps/RT_WaterH.RT_WaterH");
};

USimulationSubsystem::USimulationSubsystem()
{
	WaterHeightmapRT = ConstructorHelpers::FObjectFinderOptional<UTextureRenderTarget2D>(
		FSimulationSubsystemLocal::WaterHeightmapRTFilename).Get();

	if (auto SimFxModule = FSimFXModule::Get())
	{
		SimFxModule->WaterSystem.SetWaterHeightmapRT(WaterHeightmapRT);
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