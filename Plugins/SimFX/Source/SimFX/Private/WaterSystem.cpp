// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaterSystem.h"

bool FWaterSystem::BuildTextures(FPostOpaqueRenderParameters& Parameters)
{
	if (!WaterHeightTex.IsValid())
	{
		FRHIResourceCreateInfo CreateInfo;
		WaterHeightTex = RHICreateTexture2D(
			DispatchParams.WaterHeightmapRT->SizeX,
			DispatchParams.WaterHeightmapRT->SizeY,
			DispatchParams.WaterHeightmapRT->GetFormat(),
			1,
			1,
			TexCreate_ShaderResource | TexCreate_UAV,
			CreateInfo);
		if (!WaterHeightTex.IsValid())
		{
			return false;
		}
		WaterHeightTexUAV = RHICreateUnorderedAccessView(WaterHeightTex);
		if (!WaterHeightTexUAV.IsValid())
		{
			return false;
		}
	}
	return true;
}

void FWaterSystem::Init(FPostOpaqueRenderParameters& Parameters)
{

}

void FWaterSystem::Calc(FPostOpaqueRenderParameters& Parameters)
{

}

void FWaterSystem::Copy(FPostOpaqueRenderParameters& Parameters)
{
	FRHICopyTextureInfo CopyInfo;
	Parameters.RHICmdList->CopyTexture(
		WaterHeightTex, DispatchParams.WaterHeightmapRT->GetRenderTargetResource()->GetTexture2DRHI(), CopyInfo);
}

void FWaterSystem::Dispatch(FPostOpaqueRenderParameters& Parameters)
{
	FScopeLock ParamsLock(&DispatchParamsGuard);
	if (!IsValid(DispatchParams.WaterHeightmapRT))
	{
		return;
	}
	if (!IsValid(DispatchParams.TerrainHeightmapTex))
	{
		return;
	}
	if (!BuildTextures(Parameters))
	{
		return;
	}
	if (NeedInit)
	{
		Init(Parameters);
		NeedInit = false;
	}
	Calc(Parameters);
	Copy(Parameters);
}