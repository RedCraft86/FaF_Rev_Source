// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/AsyncSetViewTarget.h"
#include "Kismet/GameplayStatics.h"

UAsyncSetViewTarget* UAsyncSetViewTarget::SetViewTargetAsync(UObject* WorldContextObject, AActor* NewTarget,
	const float BlendTime, const EViewTargetBlendFunction BlendFunc, const float BlendExp, const bool bLockOutgoing)
{
	CREATE_TASK
	Task->SetWorldContext(WorldContextObject);
	Task->_NewViewTarget = NewTarget;
	Task->_BlendTime = BlendTime;
	Task->_BlendFunc = BlendFunc;
	Task->_BlendExp = BlendExp;
	Task->_bLockOutgoing = bLockOutgoing;
	return Task;
}

void UAsyncSetViewTarget::Activate()
{
	CHECK_RUNNING
	
	// If world context does not exist or cannot access world, try to use the view target as one.
	if (!GEngine->GetWorldFromContextObject(GetWorldContext(), EGetWorldErrorMode::LogAndReturnNull))
	{
		SetWorldContext(_NewViewTarget);
	}
	
	GET_WORLD

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!IsValid(PC))
	{
		UE_KLOG_WARNING(2.0f, TEXT("Cannot get Player Controller on Async Action %s."), *GetClass()->GetName())
		FINISH_RUNNING
		return;
	}
	
	PC->SetViewTargetWithBlend(_NewViewTarget, _BlendTime, _BlendFunc, _BlendExp, _bLockOutgoing);

	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, this, &UAsyncSetViewTarget::OnFinishChanging,
		_BlendTime + UE_KINDA_SMALL_NUMBER, false);
}

void UAsyncSetViewTarget::OnFinishChanging()
{
	OnFinished.Broadcast();
	FINISH_RUNNING
}
