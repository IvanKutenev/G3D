// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaterSystem.h"

#include "RenderCore/Public/RenderGraphUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GlobalShader.h"

namespace FWaterSystemLocal
{
	bool CreateTex2d(FTexture2DRHIRef& Tex, FUnorderedAccessViewRHIRef& TexUAV, uint32 SizeX, uint32 SizeY, uint8 Format)
	{
		if (!Tex.IsValid())
		{
			FRHIResourceCreateInfo CreateInfo;
			Tex = RHICreateTexture2D(
				SizeX,
				SizeY,
				Format,
				1,
				1,
				TexCreate_ShaderResource | TexCreate_UAV,
				CreateInfo);
			if (!Tex.IsValid())
			{
				return false;
			}
			TexUAV = RHICreateUnorderedAccessView(Tex);
			if (!TexUAV.IsValid())
			{
				return false;
			}
		}
		return true;
	}
	void SetDefines(FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("GRID_SZ"), GRID_SZ);
		OutEnvironment.SetDefine(TEXT("TEX_SZ"), TEX_SZ);
		OutEnvironment.SetDefine(TEXT("DELTA_TIME"), DELTA_TIME);
		OutEnvironment.SetDefine(TEXT("DELTA_X"), DELTA_X);
		OutEnvironment.SetDefine(TEXT("TERRAIN_SCALE"), TERRAIN_SCALE);
		OutEnvironment.SetDefine(TEXT("GRAVITY_ACC"), GRAVITY_ACC);
		OutEnvironment.SetDefine(TEXT("ALPHA"), ALPHA);
	}
};

class SIMFX_API FWaterInitHeight : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FWaterInitHeight);
	SHADER_USE_PARAMETER_STRUCT(FWaterInitHeight, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gTerrainHeightTexRO)
		SHADER_PARAMETER_UAV(RWTexture2D<float>, gWaterHeightTexRW)
	END_SHADER_PARAMETER_STRUCT()
public:
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		FWaterSystemLocal::SetDefines(OutEnvironment);
	}
};

class SIMFX_API FWaterInitVelocity : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FWaterInitVelocity);
	SHADER_USE_PARAMETER_STRUCT(FWaterInitVelocity, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gTerrainHeightTexRO)
		SHADER_PARAMETER_UAV(RWTexture2D<float2>, gWaterVelocityTexRW)
	END_SHADER_PARAMETER_STRUCT()
public:
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		FWaterSystemLocal::SetDefines(OutEnvironment);
	}
};

class SIMFX_API FWaterCalcHeight : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FWaterCalcHeight);
	SHADER_USE_PARAMETER_STRUCT(FWaterCalcHeight, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(float, gTime)
		SHADER_PARAMETER_TEXTURE(Texture2D<float2>, gWaterVelocityTexRO)
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gTerrainHeightTexRO)
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gWaterHeightPrevTexRO)
		SHADER_PARAMETER_UAV(RWTexture2D<float>, gWaterHeightTexRW)
	END_SHADER_PARAMETER_STRUCT()
public:
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		FWaterSystemLocal::SetDefines(OutEnvironment);
	}
};

class SIMFX_API FWaterCalcVelocity : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FWaterCalcVelocity);
	SHADER_USE_PARAMETER_STRUCT(FWaterCalcVelocity, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gTerrainHeightTexRO)
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gWaterHeightTexRO)
		SHADER_PARAMETER_TEXTURE(Texture2D<float2>, gWaterVelocityPrevTexRO)
		SHADER_PARAMETER_UAV(RWTexture2D<float2>, gWaterVelocityTexRW)
	END_SHADER_PARAMETER_STRUCT()
public:
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		FWaterSystemLocal::SetDefines(OutEnvironment);
	}
};

class SIMFX_API FWaterCalcNormal : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FWaterCalcNormal);
	SHADER_USE_PARAMETER_STRUCT(FWaterCalcNormal, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gTerrainHeightTexRO)
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gWaterHeightTexRO)
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, gWaterNormalTexRW)
		SHADER_PARAMETER_SAMPLER(SamplerState, gBilinearSampler)
	END_SHADER_PARAMETER_STRUCT()
public:
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		FWaterSystemLocal::SetDefines(OutEnvironment);
	}
};

class SIMFX_API FWaterCalcFoam : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FWaterCalcFoam);
	SHADER_USE_PARAMETER_STRUCT(FWaterCalcFoam, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE(Texture2D<float4>, gWaterNormalTexRO)
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gWaterFoamPrevTexRO)
		SHADER_PARAMETER_UAV(RWTexture2D<float>, gWaterFoamTexRW)
		SHADER_PARAMETER_SAMPLER(SamplerState, gBilinearSampler)
	END_SHADER_PARAMETER_STRUCT()
public:
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		FWaterSystemLocal::SetDefines(OutEnvironment);
	}
};

IMPLEMENT_GLOBAL_SHADER(FWaterInitHeight, "/Shaders/WaterInitHeight.usf", "InitHeightCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FWaterInitVelocity, "/Shaders/WaterInitVelocity.usf", "InitVelocityCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FWaterCalcHeight, "/Shaders/WaterCalcHeight.usf", "CalcHeightCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FWaterCalcVelocity, "/Shaders/WaterCalcVelocity.usf", "CalcVelocityCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FWaterCalcNormal, "/Shaders/WaterCalcNormal.usf", "CalcNormalCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FWaterCalcFoam, "/Shaders/WaterCalcFoam.usf", "CalcFoamCS", SF_Compute);

void FWaterSystem::Swap()
{
	WaterHeightTex.Swap(WaterHeightPrevTex);
	WaterHeightTexUAV.Swap(WaterHeightPrevTexUAV);
	WaterVelocityTex.Swap(WaterVelocityPrevTex);
	WaterVelocityTexUAV.Swap(WaterVelocityPrevTexUAV);
	WaterFoamTex.Swap(WaterFoamPrevTex);
	WaterFoamTexUAV.Swap(WaterFoamPrevTexUAV);
}

void FWaterSystem::ClearData(FPostOpaqueRenderParameters& Parameters)
{
	Parameters.RHICmdList->ClearUAVFloat(WaterFoamTexUAV, FVector4(0.0f));
	Parameters.RHICmdList->ClearUAVFloat(WaterFoamPrevTexUAV, FVector4(0.0f));
}

void FWaterSystem::InitHeight(FPostOpaqueRenderParameters& Parameters)
{
	TShaderMapRef<FWaterInitHeight> InitHeightCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!InitHeightCS.IsValid())
	{
		return;
	}
	FWaterInitHeight::FParameters InitHeightPassParams;
	InitHeightPassParams.gTerrainHeightTexRO = DispatchParams.TerrainHeightMapTex->GetResource()->GetTexture2DRHI();
	InitHeightPassParams.gWaterHeightTexRW = WaterHeightTexUAV;
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		InitHeightCS,
		InitHeightPassParams,
		FIntVector(
			TEX_SZ / 8 + 1,
			TEX_SZ / 8 + 1,
			1
		)
	);
}

void FWaterSystem::InitVelocity(FPostOpaqueRenderParameters& Parameters)
{
	TShaderMapRef<FWaterInitVelocity> InitVelocityCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!InitVelocityCS.IsValid())
	{
		return;
	}
	FWaterInitVelocity::FParameters InitVelocityPassParams;
	InitVelocityPassParams.gTerrainHeightTexRO = DispatchParams.TerrainHeightMapTex->GetResource()->GetTexture2DRHI();
	InitVelocityPassParams.gWaterVelocityTexRW = WaterVelocityTexUAV;
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		InitVelocityCS,
		InitVelocityPassParams,
		FIntVector(
			(TEX_SZ + 1) / 8 + 1,
			(TEX_SZ + 1) / 8 + 1,
			1
		)
	);
}

void FWaterSystem::CalcHeight(FPostOpaqueRenderParameters& Parameters)
{
	TShaderMapRef<FWaterCalcHeight> CalcHeightCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!CalcHeightCS.IsValid())
	{
		return;
	}
	FWaterCalcHeight::FParameters CalcHeightPassParams;
	CalcHeightPassParams.gTime = Time;
	CalcHeightPassParams.gWaterVelocityTexRO = WaterVelocityPrevTex;
	CalcHeightPassParams.gTerrainHeightTexRO = DispatchParams.TerrainHeightMapTex->GetResource()->GetTexture2DRHI();
	CalcHeightPassParams.gWaterHeightPrevTexRO = WaterHeightPrevTex;
	CalcHeightPassParams.gWaterHeightTexRW = WaterHeightTexUAV;
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		CalcHeightCS,
		CalcHeightPassParams,
		FIntVector(
			TEX_SZ / 8 + 1,
			TEX_SZ / 8 + 1,
			1
		)
	);

	FRHICopyTextureInfo CopyInfo;
	Parameters.RHICmdList->CopyTexture(
		WaterHeightTex,
		DispatchParams.WaterHeightMapRT->GetRenderTargetResource()->GetTexture2DRHI(),
		CopyInfo
	);
}

void FWaterSystem::CalcVelocity(FPostOpaqueRenderParameters& Parameters)
{
	TShaderMapRef<FWaterCalcVelocity> CalcVelocityCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!CalcVelocityCS.IsValid())
	{
		return;
	}
	FWaterCalcVelocity::FParameters CalcVelocityPassParams;
	CalcVelocityPassParams.gWaterHeightTexRO = WaterHeightTex;
	CalcVelocityPassParams.gTerrainHeightTexRO = DispatchParams.TerrainHeightMapTex->GetResource()->GetTexture2DRHI();
	CalcVelocityPassParams.gWaterVelocityPrevTexRO = WaterVelocityPrevTex;
	CalcVelocityPassParams.gWaterVelocityTexRW = WaterVelocityTexUAV;
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		CalcVelocityCS,
		CalcVelocityPassParams,
		FIntVector(
			(TEX_SZ + 1) / 8 + 1,
			(TEX_SZ + 1) / 8 + 1,
			1
		)
	);

	FRHICopyTextureInfo CopyInfo;
	Parameters.RHICmdList->CopyTexture(
		WaterVelocityTex,
		DispatchParams.WaterFlowMapRT->GetRenderTargetResource()->GetTexture2DRHI(),
		CopyInfo
	);
}

void FWaterSystem::CalcNormal(FPostOpaqueRenderParameters& Parameters)
{
	TShaderMapRef<FWaterCalcNormal> CalcNormalCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!CalcNormalCS.IsValid())
	{
		return;
	}
	FWaterCalcNormal::FParameters CalcNormalPassParams;
	CalcNormalPassParams.gTerrainHeightTexRO = DispatchParams.TerrainHeightMapTex->GetResource()->GetTexture2DRHI();
	CalcNormalPassParams.gWaterHeightTexRO = WaterHeightTex;
	CalcNormalPassParams.gWaterNormalTexRW = WaterNormalTexUAV;
	CalcNormalPassParams.gBilinearSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		CalcNormalCS,
		CalcNormalPassParams,
		FIntVector(
			TEX_SZ / 8 + 1,
			TEX_SZ / 8 + 1,
			1
		)
	);

	FRHICopyTextureInfo CopyInfo;
	Parameters.RHICmdList->CopyTexture(
		WaterNormalTex,
		DispatchParams.WaterNormalMapRT->GetRenderTargetResource()->GetTexture2DRHI(),
		CopyInfo
	);
}

void FWaterSystem::CalcFoam(FPostOpaqueRenderParameters& Parameters)
{
	TShaderMapRef<FWaterCalcFoam> CalcFoamCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!CalcFoamCS.IsValid())
	{
		return;
	}
	FWaterCalcFoam::FParameters CalcFoamPassParams;
	CalcFoamPassParams.gWaterNormalTexRO = WaterNormalTex;
	CalcFoamPassParams.gWaterFoamPrevTexRO = WaterFoamPrevTex;
	CalcFoamPassParams.gWaterFoamTexRW = WaterFoamTexUAV;
	CalcFoamPassParams.gBilinearSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		CalcFoamCS,
		CalcFoamPassParams,
		FIntVector(
			TEX_SZ / 8 + 1,
			TEX_SZ / 8 + 1,
			1
		)
	);

	FRHICopyTextureInfo CopyInfo;
	Parameters.RHICmdList->CopyTexture(
		WaterFoamTex,
		DispatchParams.WaterFoamMapRT->GetRenderTargetResource()->GetTexture2DRHI(),
		CopyInfo
	);
}

void FWaterSystem::OnRegister()
{
	NeedInit = true;
}

void FWaterSystem::OnUnregister()
{
	SetTerrainHeightMapTex();
	SetWaterHeightMapRT();
	SetWaterFlowMapRT();
	SetWaterNormalMapRT();
	SetWaterFoamMapRT();
}

bool FWaterSystem::IsInitialized()
{
	if (!IsValid(DispatchParams.TerrainHeightMapTex))
	{
		return false;
	}
	if (!IsValid(DispatchParams.WaterHeightMapRT))
	{
		return false;
	}
	if (!IsValid(DispatchParams.WaterFlowMapRT))
	{
		return false;
	}
	if (!IsValid(DispatchParams.WaterNormalMapRT))
	{
		return false;
	}
	if (!IsValid(DispatchParams.WaterFoamMapRT))
	{
		return false;
	}
	return true;
}

bool FWaterSystem::BuildResourceIfNeeded(FPostOpaqueRenderParameters& Parameters)
{
	if (!FWaterSystemLocal::CreateTex2d(
		WaterHeightTex,
		WaterHeightTexUAV,
		TEX_SZ,
		TEX_SZ,
		EPixelFormat::PF_R32_FLOAT))
	{
		return false;
	}
	if (!FWaterSystemLocal::CreateTex2d(
		WaterHeightPrevTex,
		WaterHeightPrevTexUAV,
		TEX_SZ,
		TEX_SZ,
		EPixelFormat::PF_R32_FLOAT))
	{
		return false;
	}
	if (!FWaterSystemLocal::CreateTex2d(
		WaterVelocityTex,
		WaterVelocityTexUAV,
		TEX_SZ + 1,
		TEX_SZ + 1,
		EPixelFormat::PF_G32R32F))
	{
		return false;
	}
	if (!FWaterSystemLocal::CreateTex2d(
		WaterVelocityPrevTex,
		WaterVelocityPrevTexUAV,
		TEX_SZ + 1,
		TEX_SZ + 1,
		EPixelFormat::PF_G32R32F))
	{
		return false;
	}
	if (!FWaterSystemLocal::CreateTex2d(
		WaterFoamTex,
		WaterFoamTexUAV,
		TEX_SZ,
		TEX_SZ,
		EPixelFormat::PF_R16F))
	{
		return false;
	}
	if (!FWaterSystemLocal::CreateTex2d(
		WaterFoamPrevTex,
		WaterFoamPrevTexUAV,
		TEX_SZ,
		TEX_SZ,
		EPixelFormat::PF_R16F))
	{
		return false;
	}
	if (!FWaterSystemLocal::CreateTex2d(
		WaterNormalTex,
		WaterNormalTexUAV,
		TEX_SZ,
		TEX_SZ,
		EPixelFormat::PF_FloatRGBA))
	{
		return false;
	}
	return true;
}

void FWaterSystem::Compute(FPostOpaqueRenderParameters& Parameters)
{
	if (NeedInit)
	{
		ClearData(Parameters);
		InitHeight(Parameters);
		InitVelocity(Parameters);
		Time = 0.0f;
		NeedInit = false;
	}
	Swap();
	CalcHeight(Parameters);
	CalcVelocity(Parameters);
	CalcNormal(Parameters);
	CalcFoam(Parameters);
	Time += DELTA_TIME;
}