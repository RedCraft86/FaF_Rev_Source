﻿// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "CCTVMonitor.h"

ACCTVMonitor::ACCTVMonitor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACCTVMonitor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACCTVMonitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
