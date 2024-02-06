#pragma once
 
#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineModule.h"
#include "GenericPlatform/GenericPlatformMisc.h"
 
struct SIMFX_API FDispatchParams
{
	UTextureRenderTarget2D* WaterHeightmapRT;
};
 
class SIMFX_API FWaterSystem {
public:
	void Register()
	{
		RendererDelegateHandle = GetRendererModule().RegisterPostOpaqueRenderDelegate(
			FPostOpaqueRenderDelegate::CreateRaw(this, &FWaterSystem::Dispatch));
	}
	void Unregister()
	{
		if (RendererDelegateHandle.IsValid())
		{
			GetRendererModule().RemovePostOpaqueRenderDelegate(RendererDelegateHandle);
			RendererDelegateHandle.Reset();
		}
	}
	void SetWaterHeightmapRT(UTextureRenderTarget2D* WaterHeightmapRT)
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (!IsValid(WaterHeightmapRT))
		{
			return;
		}
		if (IsValid(DispatchParams.WaterHeightmapRT))
		{
			DispatchParams.WaterHeightmapRT->RemoveFromRoot();
		}
		WaterHeightmapRT->AddToRoot();
		DispatchParams.WaterHeightmapRT = WaterHeightmapRT;
	}

private:
	void Dispatch(
		FPostOpaqueRenderParameters& Parameters
	);

private:
	FCriticalSection DispatchParamsGuard;
	FDispatchParams DispatchParams;
	FDelegateHandle RendererDelegateHandle;
};
