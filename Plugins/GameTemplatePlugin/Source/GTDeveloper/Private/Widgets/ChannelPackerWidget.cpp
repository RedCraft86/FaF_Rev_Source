// Copyright (C) RedCraft86. All Rights Reserved.

#include "Widgets/ChannelPackerWidget.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "EditorAssetLibrary.h"

#define GET_INPUT(Type) (IsValid(Input##Type) ? Input##Type : DefaultTexture)

UGTChannelPackerWidget::UGTChannelPackerWidget(const FObjectInitializer& ObjectInitializer)
	: UEditorUtilityWidget(ObjectInitializer), PackingButton(nullptr), UnpackingButton(nullptr),
	ProcessButton(nullptr), ProcessLabel(nullptr), PackerWidgets(nullptr),
	ImageRGBA(nullptr), ImageR(nullptr), ImageG(nullptr), ImageB(nullptr), ImageA(nullptr),
	InputR(nullptr), InputG(nullptr), InputB(nullptr), InputA(nullptr), InputRGBA(nullptr),
	CompressionSettings(TC_Masks), MipGenSettings(TMGS_NoMipmaps), bSRGB(false), MaxSize(2048),
	ExportPath(TEXT("/Game")), AssetName(TEXT("T_PackerOutput")),
	MIDOutputRGBA(nullptr), MIDOutputR(nullptr), MIDOutputG(nullptr), MIDOutputB(nullptr), MIDOutputA(nullptr)
{
	const ConstructorHelpers::FObjectFinder<UTexture2D> TextureFinder(TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture"));
	if (TextureFinder.Succeeded()) DefaultTexture = TextureFinder.Object;
	else DefaultTexture = LoadObject<UTexture2D>(nullptr, TEXT("/GameTemplatePlugin/Assets/ChannelPacker/T_Default.T_Default"));
	if (DefaultTexture)
	{
		InputR = DefaultTexture;
		InputG = DefaultTexture;
		InputB = DefaultTexture;
		InputA = DefaultTexture;
		InputRGBA = DefaultTexture;
	}
	
	const ConstructorHelpers::FObjectFinder<UMaterialInterface> PackerFinder(
		TEXT("/GameTemplatePlugin/Assets/ChannelPacker/M_PackerMaterial.M_PackerMaterial"));
	if (PackerFinder.Succeeded()) PackerMaterial = PackerFinder.Object;

	const ConstructorHelpers::FObjectFinder<UMaterialInterface> UnpackerFinder(
		TEXT("/GameTemplatePlugin/Assets/ChannelPacker/M_UnpackerMaterial.M_UnpackerMaterial"));
	if (UnpackerFinder.Succeeded()) UnpackerMaterial = UnpackerFinder.Object;
}

void UGTChannelPackerWidget::Reset()
{
	InputR = DefaultTexture;
	InputG = DefaultTexture;
	InputB = DefaultTexture;
	InputA = DefaultTexture;
	InputRGBA = DefaultTexture;
	CompressionSettings = TC_Masks;
	MipGenSettings = TMGS_NoMipmaps;
	bSRGB = false;
	MaxSize = 2048;
	ExportPath.Path = TEXT("/Game");
	AssetName = TEXT("T_PackerOutput");
	UpdateMaterials();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGTChannelPackerWidget::PackingButtonEvent()
{
	PackerWidgets->SetActiveWidgetIndex(0);
	ProcessLabel->SetText(INVTEXT("Pack Textures"));
	PackingButton->SetIsEnabled(false);
	UnpackingButton->SetIsEnabled(true);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGTChannelPackerWidget::UnpackingButtonEvent()
{
	PackerWidgets->SetActiveWidgetIndex(1);
	ProcessLabel->SetText(INVTEXT("Unpack Texture"));
	UnpackingButton->SetIsEnabled(false);
	PackingButton->SetIsEnabled(true);
}

void UGTChannelPackerWidget::ProcessButtonEvent()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	if (PackerWidgets->GetActiveWidgetIndex() == 0)
	{
		ExportPacked();
	}
	else
	{
		ExportUnpacked();
	}
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UGTChannelPackerWidget::ExportPacked()
{
	const FVector2D Size = GetFinalRGBASize();
	UTextureRenderTarget2D* RT = UKismetRenderingLibrary::CreateRenderTarget2D(this,
		Size.X, Size.Y, RTF_RGBA8);

	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RT, MIDOutputRGBA);
	SaveTextureAsset(RT, TEXT("RGBA"), IsValid(InputA));
}

void UGTChannelPackerWidget::ExportUnpacked()
{
	const UTexture2D* RGBATex = GET_INPUT(RGBA);
	UTextureRenderTarget2D* RT = UKismetRenderingLibrary::CreateRenderTarget2D(this,
		RGBATex->GetSizeX(), RGBATex->GetSizeY(), RTF_RGBA8);

	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RT, MIDOutputR);
	SaveTextureAsset(RT, TEXT("R"), false);
	
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RT, MIDOutputG);
	SaveTextureAsset(RT, TEXT("G"), false);

	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RT, MIDOutputB);
	SaveTextureAsset(RT, TEXT("B"), false);

	if (InputRGBA && InputRGBA->HasAlphaChannel())
	{
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RT, MIDOutputA);
		SaveTextureAsset(RT, TEXT("A"), false);
	}
}

void UGTChannelPackerWidget::UpdateMaterials() const
{
	if (IsValid(MIDOutputRGBA))
	{
		MIDOutputRGBA->SetTextureParameterValue("R", GET_INPUT(R));
		MIDOutputRGBA->SetTextureParameterValue("G", GET_INPUT(G));
		MIDOutputRGBA->SetTextureParameterValue("B", GET_INPUT(B));
		MIDOutputRGBA->SetTextureParameterValue("A", GET_INPUT(A));
		MIDOutputRGBA->SetScalarParameterValue("HasAlpha", IsValid(InputA) ? 1.0f : 0.0f);
		//ImageRGBA->SetDesiredSizeOverride(GetFinalRGBASize());
		ImageRGBA->SetBrushFromMaterial(MIDOutputRGBA);
	}
	
	UTexture2D* RGBATex = GET_INPUT(RGBA);
	if (IsValid(MIDOutputR))
	{
		MIDOutputR->SetTextureParameterValue("RGBA", RGBATex);
		MIDOutputR->SetScalarParameterValue("Value", 0.0f);
		//ImageR->SetDesiredSizeOverride({(float)RGBATex->GetSizeX(), (float)RGBATex->GetSizeY()});
		ImageR->SetBrushFromMaterial(MIDOutputR);
	}
	
	if (IsValid(MIDOutputG))
	{
		MIDOutputG->SetTextureParameterValue("RGBA", RGBATex);
		MIDOutputG->SetScalarParameterValue("Value", 1.0f);
		//ImageG->SetDesiredSizeOverride({(float)RGBATex->GetSizeX(), (float)RGBATex->GetSizeY()});
		ImageG->SetBrushFromMaterial(MIDOutputG);
	}
	
	if (IsValid(MIDOutputB))
	{
		MIDOutputB->SetTextureParameterValue("RGBA", RGBATex);
		MIDOutputB->SetScalarParameterValue("Value", 2.0f);
		//ImageB->SetDesiredSizeOverride({(float)RGBATex->GetSizeX(), (float)RGBATex->GetSizeY()});
		ImageB->SetBrushFromMaterial(MIDOutputB);
	}
	
	if (IsValid(MIDOutputA))
	{
		MIDOutputA->SetTextureParameterValue("RGBA", RGBATex);
		MIDOutputA->SetScalarParameterValue("Value", 3.0f);
		//ImageA->SetDesiredSizeOverride({(float)RGBATex->GetSizeX(), (float)RGBATex->GetSizeY()});
		ImageA->SetBrushFromMaterial(MIDOutputA);
	}
}

FVector2D UGTChannelPackerWidget::GetFinalRGBASize() const
{
	FVector2D Output(8.0f);
	if (const UTexture2D* R = GET_INPUT(R))
	{
		if (Output.X < R->GetSizeX()) Output.X = R->GetSizeX();
		if (Output.Y < R->GetSizeY()) Output.Y = R->GetSizeY();
	}
	if (const UTexture2D* G = GET_INPUT(G))
	{
		if (Output.X < G->GetSizeX()) Output.X = G->GetSizeX();
		if (Output.Y < G->GetSizeY()) Output.Y = G->GetSizeY();
	}
	if (const UTexture2D* B = GET_INPUT(B))
	{
		if (Output.X < B->GetSizeX()) Output.X = B->GetSizeX();
		if (Output.Y < B->GetSizeY()) Output.Y = B->GetSizeY();
	}
	if (const UTexture2D* A = GET_INPUT(A))
	{
		if (Output.X < A->GetSizeX()) Output.X = A->GetSizeX();
		if (Output.Y < A->GetSizeY()) Output.Y = A->GetSizeY();
	}
	return Output;
}

void UGTChannelPackerWidget::SaveTextureAsset(UTextureRenderTarget2D* RenderTarget, const FString& Suffix, const bool bHasAlpha)
{
	if (!RenderTarget) return;

	ExportPath.Path = ExportPath.Path.StartsWith(TEXT("/Game")) ? ExportPath.Path : TEXT("/Game");
	FString PathName = ExportPath.Path  / FPaths::MakeValidFileName(
	FString::Printf(TEXT("%s_%s"), *AssetName, *Suffix), *TEXT("_"));
	FPaths::ValidatePath(PathName);

	if (UEditorAssetLibrary::DoesAssetExist(PathName))
	{
		uint8 idx = 0;
		while (UEditorAssetLibrary::DoesAssetExist(FString::Printf(TEXT("%s_%02d"), *PathName, idx)) && idx < 254)
		{
			idx++;
		}

		PathName += FString::Printf(TEXT("_%02d"), idx);
	}

	if (UTexture2D* OutTexture = UKismetRenderingLibrary::RenderTargetCreateStaticTexture2DEditorOnly(
		RenderTarget, PathName, CompressionSettings, MipGenSettings))
	{
		UEditorAssetLibrary::SaveLoadedAsset(OutTexture, false);
		
		OutTexture->CompressionNoAlpha = !bHasAlpha;
		OutTexture->MaxTextureSize = MaxSize;
		OutTexture->SRGB = bSRGB;
		
		UEditorAssetLibrary::SaveLoadedAsset(OutTexture, false);
	}
}

void UGTChannelPackerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!ExportPath.Path.StartsWith(TEXT("/Game")))
	{
		ExportPath.Path = TEXT("/Game");
	}
}

void UGTChannelPackerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PackingButton->SetIsEnabled(false);
	PackingButton->OnClicked.AddDynamic(this, &UGTChannelPackerWidget::PackingButtonEvent);
	UnpackingButton->OnClicked.AddDynamic(this, &UGTChannelPackerWidget::UnpackingButtonEvent);
	ProcessButton->OnClicked.AddDynamic(this, &UGTChannelPackerWidget::ProcessButtonEvent);
	ProcessLabel->SetText(INVTEXT("Pack Textures"));
	PackerWidgets->SetActiveWidgetIndex(0);

	if (!IsValid(MIDOutputRGBA)) MIDOutputRGBA = UMaterialInstanceDynamic::Create(PackerMaterial, this);
	if (!IsValid(MIDOutputR)) MIDOutputR = UMaterialInstanceDynamic::Create(UnpackerMaterial, this);
	if (!IsValid(MIDOutputG)) MIDOutputG = UMaterialInstanceDynamic::Create(UnpackerMaterial, this);
	if (!IsValid(MIDOutputB)) MIDOutputB = UMaterialInstanceDynamic::Create(UnpackerMaterial, this);
	if (!IsValid(MIDOutputA)) MIDOutputA = UMaterialInstanceDynamic::Create(UnpackerMaterial, this);
	UpdateMaterials();
}

void UGTChannelPackerWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (PackingButton) PackingButton->OnClicked.RemoveDynamic(this, &UGTChannelPackerWidget::PackingButtonEvent);
	if (UnpackingButton) UnpackingButton->OnClicked.RemoveDynamic(this, &UGTChannelPackerWidget::UnpackingButtonEvent);
	if (ProcessButton) ProcessButton->OnClicked.RemoveDynamic(this, &UGTChannelPackerWidget::ProcessButtonEvent);
}
