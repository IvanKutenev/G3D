#pragma once
 
#include "SimulationSystem.h"
#include "Engine/TextureRenderTarget2D.h"
 
struct SIMFX_API FDispatchParams
{
	UTexture2D* TerrainHeightMapTex;
	UTextureRenderTarget2D* WaterHeightMapRT;
	UTextureRenderTarget2D* WaterNormalMapRT;
	UTextureRenderTarget2D* WaterFoamMapRT;
	UTextureRenderTarget2D* WaterFlowMapRT;
};
 
class SIMFX_API FWaterSystem : public FSimulationSystem {
public:
	void SetTerrainHeightMapTex(UTexture2D* TerrainHeightMapTex = nullptr)
	{
		SetUParam(DispatchParams.TerrainHeightMapTex, TerrainHeightMapTex);
	}
	void SetWaterHeightMapRT(UTextureRenderTarget2D* WaterHeightMapRT = nullptr)
	{
		SetUParam(DispatchParams.WaterHeightMapRT, WaterHeightMapRT);
	}
	void SetWaterFlowMapRT(UTextureRenderTarget2D* WaterFlowMapRT = nullptr)
	{
		SetUParam(DispatchParams.WaterFlowMapRT, WaterFlowMapRT);
	}
	void SetWaterNormalMapRT(UTextureRenderTarget2D* WaterNormalMapRT = nullptr)
	{
		SetUParam(DispatchParams.WaterNormalMapRT, WaterNormalMapRT);
	}
	void SetWaterFoamMapRT(UTextureRenderTarget2D* WaterFoamMapRT = nullptr)
	{
		SetUParam(DispatchParams.WaterFoamMapRT, WaterFoamMapRT);
	}

protected:
	virtual bool IsInitialized() override;
	virtual bool BuildResourceIfNeeded(FPostOpaqueRenderParameters& Parameters) override;
	virtual void Compute(FPostOpaqueRenderParameters& Parameters) override;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

private:
	void Init(FPostOpaqueRenderParameters& Parameters);
	void Calc(FPostOpaqueRenderParameters& Parameters);
	void Copy(FPostOpaqueRenderParameters& Parameters);

private:
	bool NeedInit = true;
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
