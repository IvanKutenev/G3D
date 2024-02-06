#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Subsystems/WorldSubsystem.h"
#include "SimulationSubsystem.generated.h"

UCLASS()
class USimulationSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    USimulationSubsystem();
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
private:
    UPROPERTY()
    UTextureRenderTarget2D* WaterHeightmapRT;
};