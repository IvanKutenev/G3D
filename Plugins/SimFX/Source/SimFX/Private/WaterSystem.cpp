// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaterSystem.h"

#include "RenderCore/Public/RenderGraphUtils.h"
#include "GlobalShader.h"

class SIMFX_API FWaterCalc : public FGlobalShader
{
public:

	DECLARE_GLOBAL_SHADER(FWaterCalc);
	SHADER_USE_PARAMETER_STRUCT(FWaterCalc, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, WaterHeightTexRW)
	END_SHADER_PARAMETER_STRUCT()

public:
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

IMPLEMENT_GLOBAL_SHADER(FWaterCalc, "/Shaders/WaterCalc.usf", "CalcCS", SF_Compute);

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
	TShaderMapRef<FWaterCalc> CalcCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!CalcCS.IsValid())
	{
		return;
	}
	FWaterCalc::FParameters CalcPassParams;
	CalcPassParams.WaterHeightTexRW = WaterHeightTexUAV;
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		CalcCS,
		CalcPassParams,
		FIntVector(
			WaterHeightTex->GetSizeX() / 8,
			WaterHeightTex->GetSizeY() / 8,
			1
		)
	);
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