#pragma once
 
#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineModule.h"
#include "GenericPlatform/GenericPlatformMisc.h"
 
struct SIMFX_API FDispatchParams
{
	UTexture2D* TerrainHeightMapTex;
	UTextureRenderTarget2D* WaterHeightMapRT;
	UTextureRenderTarget2D* WaterNormalMapRT;
	UTextureRenderTarget2D* WaterFoamMapRT;
	UTextureRenderTarget2D* WaterFlowMapRT;
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
	void SetTerrainHeightMapTex(UTexture2D* TerrainHeightMapTex)
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (!IsValid(TerrainHeightMapTex))
		{
			return;
		}
		if (IsValid(DispatchParams.TerrainHeightMapTex))
		{
			DispatchParams.TerrainHeightMapTex->RemoveFromRoot();
		}
		TerrainHeightMapTex->AddToRoot();
		DispatchParams.TerrainHeightMapTex = TerrainHeightMapTex;
	}
	void SetWaterHeightMapRT(UTextureRenderTarget2D* WaterHeightMapRT)
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (!IsValid(WaterHeightMapRT))
		{
			return;
		}
		if (IsValid(DispatchParams.WaterHeightMapRT))
		{
			DispatchParams.WaterHeightMapRT->RemoveFromRoot();
		}
		WaterHeightMapRT->AddToRoot();
		DispatchParams.WaterHeightMapRT = WaterHeightMapRT;
	}
	void SetWaterFlowMapRT(UTextureRenderTarget2D* WaterFlowMapRT)
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (!IsValid(WaterFlowMapRT))
		{
			return;
		}
		if (IsValid(DispatchParams.WaterFlowMapRT))
		{
			DispatchParams.WaterFlowMapRT->RemoveFromRoot();
		}
		WaterFlowMapRT->AddToRoot();
		DispatchParams.WaterFlowMapRT = WaterFlowMapRT;
	}
	void SetWaterNormalMapRT(UTextureRenderTarget2D* WaterNormalMapRT)
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (!IsValid(WaterNormalMapRT))
		{
			return;
		}
		if (IsValid(DispatchParams.WaterNormalMapRT))
		{
			DispatchParams.WaterNormalMapRT->RemoveFromRoot();
		}
		WaterNormalMapRT->AddToRoot();
		DispatchParams.WaterNormalMapRT = WaterNormalMapRT;
	}
	void SetWaterFoamMapRT(UTextureRenderTarget2D* WaterFoamMapRT)
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (!IsValid(WaterFoamMapRT))
		{
			return;
		}
		if (IsValid(DispatchParams.WaterFoamMapRT))
		{
			DispatchParams.WaterFoamMapRT->RemoveFromRoot();
		}
		WaterFoamMapRT->AddToRoot();
		DispatchParams.WaterFoamMapRT = WaterFoamMapRT;
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

	FTexture2DRHIRef WaterHeightPrevTex;
	FUnorderedAccessViewRHIRef WaterHeightPrevTexUAV;

	FTexture2DRHIRef WaterVelocityTex;
	FUnorderedAccessViewRHIRef WaterVelocityTexUAV;

	FTexture2DRHIRef WaterVelocityPrevTex;
	FUnorderedAccessViewRHIRef WaterVelocityPrevTexUAV;

	FTexture2DRHIRef WaterNormalTex;
	FUnorderedAccessViewRHIRef WaterNormalTexUAV;

	FTexture2DRHIRef WaterFoamTex;
	FUnorderedAccessViewRHIRef WaterFoamTexUAV;

	FTexture2DRHIRef WaterFoamPrevTex;
	FUnorderedAccessViewRHIRef WaterFoamPrevTexUAV;
};
