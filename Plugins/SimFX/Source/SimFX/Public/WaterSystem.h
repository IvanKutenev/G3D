#pragma once
 
#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineModule.h"
#include "GenericPlatform/GenericPlatformMisc.h"
 
struct SIMFX_API FDispatchParams
{
	UTextureRenderTarget2D* WaterHeightmapRT;
	UTexture2D* TerrainHeightmapTex;
};
 
class SIMFX_API FWaterSystem {
public:
	void Register()
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		RendererDelegateHandle = GetRendererModule().RegisterPostOpaqueRenderDelegate(
			FPostOpaqueRenderDelegate::CreateRaw(this, &FWaterSystem::Dispatch));
		NeedInit = true;
	}
	void Unregister()
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
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
	void SetTerrainHeightmapTex(UTexture2D* TerrainHeightmapTex)
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (!IsValid(TerrainHeightmapTex))
		{
			return;
		}
		if (IsValid(DispatchParams.TerrainHeightmapTex))
		{
			DispatchParams.TerrainHeightmapTex->RemoveFromRoot();
		}
		TerrainHeightmapTex->AddToRoot();
		DispatchParams.TerrainHeightmapTex = TerrainHeightmapTex;
	}

private:
	bool BuildTextures(FPostOpaqueRenderParameters& Parameters);
	void Init(FPostOpaqueRenderParameters& Parameters);
	void Calc(FPostOpaqueRenderParameters& Parameters);
	void Copy(FPostOpaqueRenderParameters& Parameters);
	void Dispatch(FPostOpaqueRenderParameters& Parameters);

private:
	bool NeedInit = true;

	FDelegateHandle RendererDelegateHandle;

	FCriticalSection DispatchParamsGuard;
	FDispatchParams DispatchParams;

	FTexture2DRHIRef WaterHeightTex;
	FUnorderedAccessViewRHIRef WaterHeightTexUAV;
};
