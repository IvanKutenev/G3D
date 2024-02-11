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
};

class SIMFX_API FWaterInitHeight : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FWaterInitHeight);
	SHADER_USE_PARAMETER_STRUCT(FWaterInitHeight, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(int, gWaterHeightTexSzX)
		SHADER_PARAMETER(int, gWaterHeightTexSzY)
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gTerrainHeightTexRO)
		SHADER_PARAMETER_UAV(RWTexture2D<float>, gWaterHeightTexRW)
	END_SHADER_PARAMETER_STRUCT()
public:
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

class SIMFX_API FWaterInitVelocity : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FWaterInitVelocity);
	SHADER_USE_PARAMETER_STRUCT(FWaterInitVelocity, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(int, gWaterVelocityTexSzX)
		SHADER_PARAMETER(int, gWaterVelocityTexSzY)
		SHADER_PARAMETER_UAV(RWTexture2D<float2>, gWaterVelocityTexRW)
	END_SHADER_PARAMETER_STRUCT()
public:
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

class SIMFX_API FWaterCalcHeight : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FWaterCalcHeight);
	SHADER_USE_PARAMETER_STRUCT(FWaterCalcHeight, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(int, gWaterHeightTexSzX)
		SHADER_PARAMETER(int, gWaterHeightTexSzY)
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
	}
};

class SIMFX_API FWaterCalcVelocity : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FWaterCalcVelocity);
	SHADER_USE_PARAMETER_STRUCT(FWaterCalcVelocity, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(int, gWaterVelocityTexSzX)
		SHADER_PARAMETER(int, gWaterVelocityTexSzY)
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gTerrainHeightTexRO)
		SHADER_PARAMETER_TEXTURE(Texture2D<float>, gWaterHeightTexRO)
		SHADER_PARAMETER_TEXTURE(Texture2D<float2>, gWaterVelocityPrevTexRO)
		SHADER_PARAMETER_UAV(RWTexture2D<float2>, gWaterVelocityTexRW)
	END_SHADER_PARAMETER_STRUCT()
public:
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

IMPLEMENT_GLOBAL_SHADER(FWaterInitHeight, "/Shaders/WaterInitHeight.usf", "InitHeightCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FWaterInitVelocity, "/Shaders/WaterInitVelocity.usf", "InitVelocityCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FWaterCalcHeight, "/Shaders/WaterCalcHeight.usf", "CalcHeightCS", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FWaterCalcVelocity, "/Shaders/WaterCalcVelocity.usf", "CalcVelocityCS", SF_Compute);

bool FWaterSystem::BuildTextures(FPostOpaqueRenderParameters& Parameters)
{
	if (!FWaterSystemLocal::CreateTex2d(
		WaterHeightTex,
		WaterHeightTexUAV,
		DispatchParams.WaterHeightmapRT->SizeX,
		DispatchParams.WaterHeightmapRT->SizeY,
		EPixelFormat::PF_R32_FLOAT))
	{
		return false;
	}
	if (!FWaterSystemLocal::CreateTex2d(
		WaterHeightPrevTex,
		WaterHeightPrevTexUAV,
		DispatchParams.WaterHeightmapRT->SizeX,
		DispatchParams.WaterHeightmapRT->SizeY,
		EPixelFormat::PF_R32_FLOAT))
	{
		return false;
	}
	if (!FWaterSystemLocal::CreateTex2d(
		WaterVelocityTex,
		WaterVelocityTexUAV,
		DispatchParams.WaterHeightmapRT->SizeX + 1,
		DispatchParams.WaterHeightmapRT->SizeY + 1,
		EPixelFormat::PF_G32R32F))
	{
		return false;
	}
	if (!FWaterSystemLocal::CreateTex2d(
		WaterVelocityPrevTex,
		WaterVelocityPrevTexUAV,
		DispatchParams.WaterHeightmapRT->SizeX + 1,
		DispatchParams.WaterHeightmapRT->SizeY + 1,
		EPixelFormat::PF_G32R32F))
	{
		return false;
	}
	return true;
}

void FWaterSystem::Init(FPostOpaqueRenderParameters& Parameters)
{
	TShaderMapRef<FWaterInitHeight> InitHeightCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!InitHeightCS.IsValid())
	{
		return;
	}
	FWaterInitHeight::FParameters InitHeightPassParams;
	InitHeightPassParams.gWaterHeightTexSzX = WaterHeightTex->GetSizeX();
	InitHeightPassParams.gWaterHeightTexSzY = WaterHeightTex->GetSizeY();
	InitHeightPassParams.gTerrainHeightTexRO = DispatchParams.TerrainHeightmapTex->GetResource()->GetTexture2DRHI();
	InitHeightPassParams.gWaterHeightTexRW = WaterHeightTexUAV;
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		InitHeightCS,
		InitHeightPassParams,
		FIntVector(
			WaterHeightTex->GetSizeX() / 8 + 1,
			WaterHeightTex->GetSizeY() / 8 + 1,
			1
		)
	);
	TShaderMapRef<FWaterInitVelocity> InitVelocityCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!InitVelocityCS.IsValid())
	{
		return;
	}
	FWaterInitVelocity::FParameters InitVelocityPassParams;
	InitVelocityPassParams.gWaterVelocityTexSzX = WaterHeightTex->GetSizeX() + 1;
	InitVelocityPassParams.gWaterVelocityTexSzY = WaterHeightTex->GetSizeY() + 1;
	InitVelocityPassParams.gWaterVelocityTexRW = WaterVelocityTexUAV;
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		InitVelocityCS,
		InitVelocityPassParams,
		FIntVector(
			(WaterHeightTex->GetSizeX() + 1) / 8 + 1,
			(WaterHeightTex->GetSizeY() + 1) / 8 + 1,
			1
		)
	);
}

void FWaterSystem::Calc(FPostOpaqueRenderParameters& Parameters)
{
	WaterHeightTex.Swap(WaterHeightPrevTex);
	WaterHeightTexUAV.Swap(WaterHeightPrevTexUAV);
	WaterVelocityTex.Swap(WaterVelocityPrevTex);
	WaterVelocityTexUAV.Swap(WaterVelocityPrevTexUAV);
	TShaderMapRef<FWaterCalcHeight> CalcHeightCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!CalcHeightCS.IsValid())
	{
		return;
	}
	static float Time = 0.0f;
	FWaterCalcHeight::FParameters CalcHeightPassParams;
	CalcHeightPassParams.gWaterHeightTexSzX = WaterHeightTex->GetSizeX();
	CalcHeightPassParams.gWaterHeightTexSzY = WaterHeightTex->GetSizeY();
	CalcHeightPassParams.gTime = Time;
	CalcHeightPassParams.gWaterVelocityTexRO = WaterVelocityPrevTex;
	CalcHeightPassParams.gTerrainHeightTexRO = DispatchParams.TerrainHeightmapTex->GetResource()->GetTexture2DRHI();
	CalcHeightPassParams.gWaterHeightPrevTexRO = WaterHeightPrevTex;
	CalcHeightPassParams.gWaterHeightTexRW = WaterHeightTexUAV;
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		CalcHeightCS,
		CalcHeightPassParams,
		FIntVector(
			WaterHeightTex->GetSizeX() / 8 + 1,
			WaterHeightTex->GetSizeY() / 8 + 1,
			1
		)
	);
	Time += 1.0f / 30.0f;
	TShaderMapRef<FWaterCalcVelocity> CalcVelocityCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	if (!CalcVelocityCS.IsValid())
	{
		return;
	}
	FWaterCalcVelocity::FParameters CalcVelocityPassParams;
	CalcVelocityPassParams.gWaterVelocityTexSzX = WaterHeightTex->GetSizeX() + 1;
	CalcVelocityPassParams.gWaterVelocityTexSzY = WaterHeightTex->GetSizeY() + 1;
	CalcVelocityPassParams.gWaterHeightTexRO = WaterHeightTex;
	CalcVelocityPassParams.gTerrainHeightTexRO = DispatchParams.TerrainHeightmapTex->GetResource()->GetTexture2DRHI();
	CalcVelocityPassParams.gWaterVelocityPrevTexRO = WaterVelocityPrevTex;
	CalcVelocityPassParams.gWaterVelocityTexRW = WaterVelocityTexUAV;
	FComputeShaderUtils::Dispatch(
		*Parameters.RHICmdList,
		CalcVelocityCS,
		CalcVelocityPassParams,
		FIntVector(
			(WaterHeightTex->GetSizeX() + 1) / 8 + 1,
			(WaterHeightTex->GetSizeY() + 1) / 8 + 1,
			1
		)
	);
}

void FWaterSystem::Copy(FPostOpaqueRenderParameters& Parameters)
{
	FRHICopyTextureInfo CopyInfo;
	Parameters.RHICmdList->CopyTexture(
		WaterHeightTex,
		DispatchParams.WaterHeightmapRT->GetRenderTargetResource()->GetTexture2DRHI(),
		CopyInfo
	);
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