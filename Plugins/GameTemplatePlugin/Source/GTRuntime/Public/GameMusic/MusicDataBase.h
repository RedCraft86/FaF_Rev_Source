// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "MusicDataBase.generated.h"

USTRUCT(BlueprintType)
struct GTRUNTIME_API FMusicChannelData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "MusicChannelData")
		TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, Category = "MusicChannelData", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float Volume;

	UPROPERTY(EditAnywhere, Category = "MusicChannelData", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float Pitch;

	UPROPERTY(EditAnywhere, Category = "MusicChannelData", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float FadeTime;

	UPROPERTY(EditAnywhere, Category = "MusicChannelData")
		bool bRememberTime;

	FMusicChannelData(): Volume(1.0f), Pitch(1.0f), FadeTime(1.0f), bRememberTime(false) {}
};

UCLASS()
class GTRUNTIME_API UMusicDataBase : public UDataAsset
{
	GENERATED_BODY()

public:

	UMusicDataBase() {}
	
	static inline TSubclassOf<UMusicDataBase> DefaultSpawnClass = StaticClass();

	UPROPERTY(EditAnywhere, Category = "WorldMusicData", meta = (TitleProperty = "Sound"))
		TArray<FMusicChannelData> BaseChannels;

#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
