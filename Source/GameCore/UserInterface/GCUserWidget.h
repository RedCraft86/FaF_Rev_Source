// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GCUserWidget.generated.h"

#define PLAY_FIRST_FRAME(Animation) \
	PlayAnimationTimeRange(Animation, 0.0f, 0.01f); \
	PlayAnimationReverse(Animation);

#define PLAY_ANIM(Animation, Direction) PlayAnimation(Animation, 0.0f, 1, EUMGSequencePlayMode::Direction);

UCLASS(Abstract, DisplayName = "Advanced User Widget Base")
class GAMECORE_API UGCUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UGCUserWidget(const FObjectInitializer& ObjectInitializer);

	static UGCUserWidget* Create(class AGCPlayerController* Controller, const TSubclassOf<UGCUserWidget> Class);

	template<typename T = UGCUserWidget>
	static T* Create(AGCPlayerController* Controller, const TSubclassOf<UGCUserWidget> Class)
	{
		return Cast<T>(Create(Controller, Class));
	}
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* FadeAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameWidget")
		uint8 WidgetZOrder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameWidget")
		bool bAutoAddToViewport;
	
	UFUNCTION(BlueprintCallable, Category = "User Interface|GameWidget")
		virtual void SetWidgetHidden(const bool bInHidden);

	UFUNCTION(BlueprintCallable, Category = "User Interface|GameWidget")
		void AddUserWidget(const bool bFade = true);
	
	UFUNCTION(BlueprintCallable, Category = "User Interface|GameWidget")
		void RemoveUserWidget(const bool bFade = true);
	
	virtual void AddWidget(const TFunction<void()>& OnFinished, const bool bFade);
	virtual void RemoveWidget(const TFunction<void()>& OnFinished, const bool bFade);
	
protected:

	UPROPERTY(Transient)
		AGCPlayerController* PlayerController;
	
	bool bShouldBeHidden;
	bool bIsRemoving;

	virtual void NativeConstruct() override;
	virtual void InitData(AGCPlayerController* Controller);
};