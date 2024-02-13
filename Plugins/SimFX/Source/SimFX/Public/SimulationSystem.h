#pragma once
 
#include "CoreMinimal.h"
#include "EngineModule.h"
#include "GenericPlatform/GenericPlatformMisc.h"
 
class SIMFX_API FSimulationSystem {
public:
	virtual ~FSimulationSystem() {}

	void Dispatch(FPostOpaqueRenderParameters& Parameters)
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (!IsInitialized())
		{
			return;
		}
		if (!BuildResourceIfNeeded(Parameters))
		{
			return;
		}
		Compute(Parameters);
	}
	void Register()
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (!RendererDelegateHandle.IsValid())
		{
			RendererDelegateHandle = GetRendererModule().RegisterPostOpaqueRenderDelegate(
				FPostOpaqueRenderDelegate::CreateRaw(this, &FSimulationSystem::Dispatch));
		}
		OnRegister();
	}
	void Unregister()
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (RendererDelegateHandle.IsValid())
		{
			GetRendererModule().RemovePostOpaqueRenderDelegate(RendererDelegateHandle);
			RendererDelegateHandle.Reset();
		}
		OnUnregister();
	}

protected:
	virtual bool IsInitialized() { return true; }
	virtual bool BuildResourceIfNeeded(FPostOpaqueRenderParameters& Parameters) { return true; }
	virtual void Compute(FPostOpaqueRenderParameters& Parameters) {}

	virtual void OnRegister() {}
	virtual void OnUnregister() {}

	template<class UParamClass>
	void SetUParam(UParamClass*& Param, UParamClass* Value = nullptr)
	{
		FScopeLock ParamsLock(&DispatchParamsGuard);
		if (IsValid(Param))
		{
			Param->RemoveFromRoot();
		}
		if (!IsValid(Value))
		{
			return;
		}
		Value->AddToRoot();
		Param = Value;
	}

private:
	FDelegateHandle RendererDelegateHandle;
	FCriticalSection DispatchParamsGuard;
};
